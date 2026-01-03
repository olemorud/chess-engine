#pragma once

#include "libc-lite.h"
#include "sys.h"

#include <stdint.h>
#include <stdatomic.h>

/* temp performance counter */
static uint64_t g_ab_node_volume = 0;

#include "engine-macros.h"
#include "engine-types.h"
#include "engine-bitboard.h"
#include "engine-board.h"
#include "engine-attack-sets.h"
#include "engine-move-generation.h"
#include "engine-tt.h"
#include "engine-evaluations.h"

/* --------------------------- MOVE SEARCH --------------------------------- */

#define SCORE_INF       1e30
#define SCORE_CHECKMATE 999.0

/* for initial ordering of moves in alphabeta search */
static void move_compute_appeal(struct move* restrict      m,
                                struct pos const* restrict pos,
                                Side8                      us,
                                Piece8                     mailbox[restrict static SQ_COUNT])
{
    /* MVV-LVA: https://www.chessprogramming.org/MVV-LVA */
    Side8 them = other_side(us);
    Piece8 const atk = mailbox[m->from];

    uint8_t n = 1;
    if (MASK_FROM_SQ(m->to) & pos->occupied[them]) {
        n += (uint8_t)piece_value[mailbox[m->to]];
    }

    uint8_t mmv_lva_bonus = 16*n - (uint8_t)piece_value[atk];

    m->appeal = mmv_lva_bonus;
}

static double board_score_heuristic(struct pos const* pos)
{
    /* this function always evaluates from white's perspective before
       eventually flipping the sign based on `pos` */
    double score = 0.0;

    Bb64 const occw = pos->occupied[SIDE_WHITE];
    Bb64 const occb = pos->occupied[SIDE_BLACK];
    Bb64 const occall = occw | occb;

    enum game_progress const gp = endgameness(pos);

    if (pos->pieces[SIDE_WHITE][PIECE_KING] == 0) score -= (double)SCORE_CHECKMATE;
    if (pos->pieces[SIDE_BLACK][PIECE_KING] == 0) score += (double)SCORE_CHECKMATE;

    for (Piece8 p = PIECE_BEGIN; p < PIECE_COUNT; ++p) {
        /* raw material value */
        score += piece_value[p] *
            ((double)bitboard_popcount(pos->pieces[SIDE_WHITE][p]) -
             (double)bitboard_popcount(pos->pieces[SIDE_BLACK][p]));

        /* very minor advantage for threat projection to break tied moves */
        if (p != PIECE_PAWN) {
            score += 0.001 * (
                (double)bitboard_popcount(non_pawn_piece_attacks(p, pos->pieces[SIDE_WHITE][p], occall))
              - (double)bitboard_popcount(non_pawn_piece_attacks(p, pos->pieces[SIDE_BLACK][p], occall)));
        }

        /* positional bonus, see evaluations.h */
        for (size_t i = 0; i < POSITIONAL_MODIFIER_COUNT; ++i) {
            score += positional_modifier(SIDE_WHITE, gp, i, p).val *
                (
                 (double)bitboard_popcount(
                    pos->pieces[SIDE_WHITE][p]
                  & positional_modifier(SIDE_WHITE, gp, i, p).area)
               - (double)bitboard_popcount(
                     pos->pieces[SIDE_BLACK][p]
                  & positional_modifier(SIDE_BLACK, gp, i, p).area)
               );
        }
    }

    /* pawns defending pieces are desired */
    score += 0.03 * (
        (double)bitboard_popcount(
            pawn_attacks_white(pos->pieces[SIDE_WHITE][PIECE_PAWN]) & occw
        )
        - (double)bitboard_popcount(
            pawn_attacks_black(pos->pieces[SIDE_BLACK][PIECE_PAWN]) & occb
        )
    );

    /* stacked pawns are bad */
    const double k = 0.30;
    for (enum file_index fi = FILE_INDEX_BEGIN; fi < FILE_INDEX_COUNT; ++fi) {
        uint64_t wstk = bitboard_popcount(pos->pieces[SIDE_WHITE][PIECE_PAWN] & FILE_MASK(fi));
        uint64_t bstk = bitboard_popcount(pos->pieces[SIDE_BLACK][PIECE_PAWN] & FILE_MASK(fi));

        score -= k * (double)(wstk - (wstk == 1));
        score += k * (double)(bstk - (bstk == 1));
    }

    double sign = (pos->moving_side == SIDE_WHITE) ? 1.0 : -1.0;

    return sign*score;
}

static
struct move moves_linear_search(struct move      moves[restrict static MOVE_MAX],
                                              size_t* restrict move_count)
{
    size_t best = 0;
    assuming(*move_count > 0);
    for (size_t i = 0; i < *move_count; ++i) {
        if (moves[i].appeal > moves[best].appeal) {
            best = i;
        }
    }

    struct move m = moves[best];
    moves[best] = moves[(*move_count) - 1];
    *move_count -= 1;

    return m;
}

/* quiescence is a deep search that only considers captures */
static
double quiesce(struct pos const* pos,
               Piece8            mailbox[restrict static SQ_COUNT],
               Side8             us,
               double            alpha,
               double            beta,
               int8_t            depth)
{
    if (pos->pieces[us][PIECE_KING] == 0) {
        return -SCORE_CHECKMATE;
    }

    Side8 const them = other_side(us);

    double score = board_score_heuristic(pos);
    double highscore = score;

    if (highscore >= beta) {
        return highscore;
    }
    if (highscore > alpha) {
        alpha = highscore;
    }

    size_t move_count = 0;

    struct move moves[MOVE_MAX];

    all_pseudolegal_moves(pos, MG_CAPTURES, us, &move_count, moves);
    if (move_count == 0) {
        /* TODO: detect stalemate */
        return -(999.0 + (double)depth);
    }
    for (size_t i = 0; i < move_count; ++i) {
        move_compute_appeal(&moves[i], pos, us, mailbox);
    }

    while (move_count) {
        struct move m = moves_linear_search(moves, &move_count);

        assuming((pos->occupied[them] | pos->ep_targets) & MASK_FROM_SQ(m.to));

        struct pos poscpy = *pos;

        Piece8 mailbox_cpy[SQ_COUNT];
        my_memcpy(mailbox_cpy, mailbox, sizeof (Piece8[SQ_COUNT]));

        /* history is irrelevant when all moves are captures */
        static struct history hist;
        hist.length = 0;
        (void)move_piece(&poscpy, &hist, mailbox_cpy, m);

        score = -quiesce(&poscpy, mailbox_cpy, them, -beta, -alpha, depth - 1);

        if (score >= beta) {
            highscore = score;
            break;
        }

        if (score > highscore) {
            highscore = score;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    return highscore;
}

static
struct search_option alphabeta_search(struct pos const* pos,
                                      struct history*   hist,
                                      struct tt*        tt,
                                      Piece8            mailbox[restrict static SQ_COUNT],
                                      Side8             us,
                                      int8_t            depth,
                                      double            alpha,
                                      double            beta,
                                      atomic_bool*      searching)
{
    g_ab_node_volume += 1;

    if (pos->pieces[us][PIECE_KING] == 0) {
        return (struct search_option) {
            .score = -(SCORE_CHECKMATE + (double)depth),
            .move = (struct move){0},
            .depth = 0,
            .hash = pos->hash,
            .init = true,
            .flag = TT_EXACT,
        };
    }

    if (depth <= 0) {
        double sc = quiesce(pos, mailbox, us, alpha, beta, 0);
        return (struct search_option){
            .score = sc,
            .move  = (struct move){0},
            .depth = 0,
            .hash  = pos->hash,
            .init  = true,
            .flag  = TT_EXACT,
        };
    }

    double const alpha_orig = alpha;

    struct move moves[MOVE_MAX];
    size_t move_count = 0;

    struct search_option tte = tt_get(tt, pos->hash);
    if (tte.init && tte.hash == pos->hash) {
        if (tte.depth >= depth) {
            if (tte.flag == TT_EXACT) {
                return tte;
            } else if (tte.flag == TT_LOWER) {
                if (tte.score > alpha) alpha = tte.score;
            } else if (tte.flag == TT_UPPER) {
                if (tte.score < beta)  beta  = tte.score;
            }

            if (alpha >= beta) {
                return tte;
            }
        }

        moves[move_count] = tte.move;
        moves[move_count].appeal = APPEAL_MAX;
        ++move_count;
    }

    double best_score = -SCORE_INF;
    struct move best_move = NULL_MOVE;

    enum move_gen_type const types[] = {MG_CAPTURES, MG_CHECKS, MG_QUIETS};
    for (size_t i = 0; i < sizeof types / sizeof *types; ++i) {
        all_pseudolegal_moves(pos, types[i], us, &move_count, moves);

        for (size_t i = 0; i < move_count; ++i) {
            move_compute_appeal(&moves[i], pos, us, mailbox);
        }

        while (move_count > 0) {
            if (!atomic_load_explicit(searching, memory_order_relaxed)) {
                return (struct search_option) { .init = false };
            }
            struct move m = moves_linear_search(moves, &move_count);

            size_t const old_hist_len = hist->length;
            struct pos pos_cpy = *pos;
            Piece8 mailbox_cpy[SQ_COUNT];
            my_memcpy(mailbox_cpy, mailbox, sizeof mailbox_cpy);

            enum move_result r = move_piece(&pos_cpy, hist, mailbox_cpy, m);

            double score;

            if (r == MR_STALEMATE || r == MR_REPEATS) {
                score = 0.0;
            } else {
                struct search_option so = alphabeta_search(&pos_cpy,
                                              hist,
                                              tt,
                                              mailbox_cpy,
                                              other_side(us),
                                              depth - 1,
                                              -beta,
                                              -alpha,
                                              searching);
                if (!so.init) {
                    hist->length = old_hist_len;
                    return (struct search_option){ .init = false, .move = NULL_MOVE };
                }
                score = -so.score;
            }

            hist->length = old_hist_len;

            if (score > best_score) {
                best_score = score;
                best_move = m;
            }
            if (score > alpha) {
                alpha = score;
            }
            if (alpha >= beta) {
                goto finish_search;
            }
        }
    }

finish_search:

    if (IS_NULL_MOVE(best_move)) {
        return (struct search_option){ .init = false, .move = NULL_MOVE };
    }

    enum tt_flag flag = TT_EXACT;
    if (best_score <= alpha_orig) {
        flag = TT_UPPER;
    }
    else if (best_score >= beta) {
        flag = TT_LOWER;
    }

    struct search_option out = (struct search_option){
        .score = best_score,
        .move  = best_move,
        .depth = depth,
        .hash  = pos->hash,
        .init  = true,
        .flag  = flag,
    };

    tt_insert(tt, pos->hash, out);

    return out;
}

static
struct search_result {struct move move; double score;}
  search(struct board* b, Side8 us, int8_t max_depth, atomic_bool* searching)
{
    struct move moves[MOVE_MAX];
    size_t move_count = 0;
    all_pseudolegal_moves(&b->pos, MG_ALL, us, &move_count, moves);

    assuming(move_count);

    struct move best_move = moves[0];

    g_ab_node_volume = 0;

    double score = 0.0;

    for (int8_t d = 1;
         d <= max_depth && atomic_load_explicit(searching, memory_order_relaxed);
         ++d)
    {
        double window = SCORE_INF; /* temp debug solution */
        double alpha = score - window;
        double beta = score + window;

        while (1) {
            struct search_option so =
                alphabeta_search(&b->pos, &b->hist, &b->tt, b->mailbox, us, d, alpha, beta, searching);

            if (atomic_load_explicit(searching, memory_order_relaxed) == false) {
                break;
            }

            if (IS_NULL_MOVE(so.move)) {
                break;
            }

            if (so.score > alpha && so.score < beta) {
                score = so.score;
                best_move = so.move;
                break;
            }

            window *= 2;

            if (so.score <= alpha) {
                alpha = -SCORE_INF;
                beta = score + window;
            } else {
                alpha = score - window;
                beta = SCORE_INF;
            }

        }
#ifdef USE_PRINTF
        fprintf(stderr, "depth: %hhd\n", d);
#endif
    }

#undef SCORE_INF

#ifdef USE_PRINTF
    fprintf(stderr, "nodes searched: %'llu\n", g_ab_node_volume);
#endif

    return (struct search_result){.move = best_move, .score = score};
}


