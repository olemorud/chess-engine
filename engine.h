#pragma once

/* WIP switches */
#define NEW_SEARCHING

#include "libc-lite.h"
#include "sys.h"

#include <stdint.h>
#include <stdatomic.h>

/* temp performance counter */
static uint64_t g_ab_node_volume = 0;
static uint64_t g_pvs_re_search = 0;
static uint64_t g_pvs_probes = 0;

#include "engine-macros.h"
#include "engine-types.h"
#include "engine-bitboard.h"
#include "engine-board.h"
#include "engine-attack-sets.h"
#include "engine-move-generation.h"
#include "engine-tt.h"
#include "engine-evaluations.h"

/* --------------------------- MOVE SEARCH --------------------------------- */

/* for initial ordering of moves in alphabeta search */
static void move_compute_appeal(struct move* restrict      m,
                                struct pos const* restrict pos,
                                Side8                      us,
                                Piece8                     mailbox[restrict static SQ_COUNT])
{
    /* MVV-LVA: https://www.chessprogramming.org/MVV-LVA */
    Side8 them = other_side(us);
    Piece8 const atk = mailbox[m->from];

    uint8_t n = 0;
    if ((MASK_FROM_SQ(m->to) & pos->pieces[them][PIECE_EMPTY]) == 0) {
        n += (uint8_t)piece_value[mailbox[m->to]];
    }

    /* TODO: remove branch */
    if (n) {
        m->appeal = (uint8_t)(16*n - piece_value[atk]);
    } else {
        m->appeal = 0;
    }
}

static Score16 board_score_heuristic(struct pos const* pos)
{
    /* this function always evaluates from white's perspective before
       eventually flipping the sign based on `pos` */
    Score16 score = 0;

    Bb64 const occw = ~pos->pieces[SIDE_WHITE][PIECE_EMPTY];
    Bb64 const occb = ~pos->pieces[SIDE_BLACK][PIECE_EMPTY];

    enum game_progress const gp = endgameness(pos);

    if (pos->pieces[SIDE_WHITE][PIECE_KING]) {
        score += SCORE_CHECKMATE;
    }
    if (pos->pieces[SIDE_BLACK][PIECE_KING]) {
        score -= SCORE_CHECKMATE;
    }

    for (Piece8 p = PIECE_BEGIN; p < PIECE_COUNT; ++p) {
        /* raw material value */
        score += piece_value[p] *
            ((Score16)bitboard_popcount(pos->pieces[SIDE_WHITE][p]) -
             (Score16)bitboard_popcount(pos->pieces[SIDE_BLACK][p]));

        /* positional bonus, see evaluations.h */
        for (size_t i = 0; i < POSITIONAL_MODIFIER_COUNT; ++i) {
            score += positional_modifier(SIDE_WHITE, gp, i, p).val *
                (
                 (Score16)bitboard_popcount(
                    pos->pieces[SIDE_WHITE][p]
                  & positional_modifier(SIDE_WHITE, gp, i, p).area)
               - (Score16)bitboard_popcount(
                     pos->pieces[SIDE_BLACK][p]
                  & positional_modifier(SIDE_BLACK, gp, i, p).area)
               );
        }
    }

    /* bishop pair */
    score += 10 * (Score16)bitboard_more_than_one(pos->pieces[SIDE_WHITE][PIECE_BISHOP]);
    score -= 10 * (Score16)bitboard_more_than_one(pos->pieces[SIDE_BLACK][PIECE_BISHOP]);

    /* pawns defending pieces are desired */
    score += 3 * (
        (Score16)bitboard_popcount(
            pawn_attacks_white(pos->pieces[SIDE_WHITE][PIECE_PAWN]) & occw
        )
        - (Score16)bitboard_popcount(
            pawn_attacks_black(pos->pieces[SIDE_BLACK][PIECE_PAWN]) & occb
        )
    );

    /* stacked pawns are bad */
    /*
    const Score16 k = 30;
    for (enum file_index fi = FILE_INDEX_BEGIN; fi < FILE_INDEX_COUNT; ++fi) {
        uint64_t wstk = bitboard_popcount(pos->pieces[SIDE_WHITE][PIECE_PAWN] & FILE_MASK(fi));
        uint64_t bstk = bitboard_popcount(pos->pieces[SIDE_BLACK][PIECE_PAWN] & FILE_MASK(fi));

        score -= k * (Score16)(wstk - (wstk == 1));
        score += k * (Score16)(bstk - (bstk == 1));
    }
    */

    Score16 sign = (pos->moving_side == SIDE_WHITE) ? 1 : -1;

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
Score16 quiesce(struct pos const*  pos,
                Piece8             mailbox[restrict static SQ_COUNT],
                Side8              us,
                Score16            alpha,
                Score16            beta,
                int8_t             depth)
{
    if (pos->pieces[us][PIECE_KING] == 0) {
        return -(SCORE_CHECKMATE + depth);
    }

    Side8 const them = other_side(us);

    Score16 score = board_score_heuristic(pos);
    Score16 highscore = score;

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
        return score;
    }

    for (size_t i = 0; i < move_count; ++i) {
        move_compute_appeal(&moves[i], pos, us, mailbox);
    }

    while (move_count) {
        struct move m = moves_linear_search(moves, &move_count);

        assuming((~pos->pieces[them][PIECE_EMPTY] | pos->ep_targets) & MASK_FROM_SQ(m.to));

        struct pos poscpy = *pos;

        Piece8 mailbox_cpy[SQ_COUNT];
        my_memcpy(mailbox_cpy, mailbox, sizeof (Piece8[SQ_COUNT]));

        /* history is irrelevant when all moves are captures */
        static struct history hist;
        hist.length = 0;
        (void)move_piece(&poscpy, us, &hist, mailbox_cpy, m);

        score = (Score16)-quiesce(&poscpy, mailbox_cpy, them, (Score16)(-beta), (Score16)(-alpha), depth - 1);

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
                                      Score16           alpha,
                                      Score16           beta
                                     #ifdef FEATURE_STOPPABLE_SEARCH
                                      ,
                                      struct searching_flag const* restrict  searching
                                     #endif
                                      )
{
    g_ab_node_volume += 1;


#ifdef FEATURE_STOPPABLE_SEARCH
    if (!searching_still(searching)) {
        return (struct search_option) { .init = false, .move = MOVE_NULL };
    }
#endif

    if (pos->pieces[us][PIECE_KING] == 0) {
        return (struct search_option) {
            .score = -(SCORE_CHECKMATE + depth),
            .move = (struct move){0},
            .depth = 0,
            .hash = pos->hash,
            .init = true,
            .flag = TT_EXACT,
        };
    }

    if (depth <= 0) {
        Score16 sc = quiesce(pos, mailbox, us, alpha, beta, 0);
        return (struct search_option){
            .score = sc,
            .move  = (struct move){0},
            .depth = 0,
            .hash  = pos->hash,
            .init  = true,
            .flag  = TT_EXACT,
        };
    }

    Score16 const alpha_orig = alpha;

    struct move moves[MOVE_MAX];
    size_t move_count = 0;

    Score16 best_score = -SCORE_INF;
    struct move best_move = MOVE_NULL;
    bool has_principal_move = false;

    struct search_option tte = tt_get(tt, pos->hash);
    if (tte.init && tte.hash == pos->hash) {
        if (tte.depth >= depth) {
            if (tte.flag == TT_EXACT) {
                assuming(tte.init);
                return tte;
            } else if (tte.flag == TT_LOWER) {
                if (tte.score > alpha) alpha = tte.score;
            } else if (tte.flag == TT_UPPER) {
                if (tte.score < beta)  beta  = tte.score;
            }

            if (alpha >= beta) {
                assuming(tte.init);
                return tte;
            }
        }

        moves[move_count] = tte.move;
        moves[move_count].appeal = APPEAL_MAX;
        ++move_count;
        has_principal_move = true;
    }

    bool first = true;

    enum move_gen_type const types[] = {MG_CAPTURES, MG_CHECKS, MG_QUIETS};
    for (size_t i = 0; i < sizeof types / sizeof *types; ++i) {
        all_pseudolegal_moves(pos, types[i], us, &move_count, moves);

        for (size_t i = (size_t)has_principal_move; i < move_count; ++i) {
            move_compute_appeal(&moves[i], pos, us, mailbox);
        }

        while (move_count > 0) {
#ifdef FEATURE_STOPPABLE_SEARCH
            if (!searching_still(searching)) {
                return (struct search_option) { .init = false };
            }
#endif
            struct move m = moves_linear_search(moves, &move_count);

            size_t const old_hist_len = hist->length;
            struct pos pos_cpy = *pos;
            Piece8 mailbox_cpy[SQ_COUNT];
            my_memcpy(mailbox_cpy, mailbox, sizeof mailbox_cpy);

            enum move_result r = move_piece(&pos_cpy, us, hist, mailbox_cpy, m);

            Score16 score;

            if (r == MR_STALEMATE || r == MR_REPEATS) {
                score = 0;
            } else {
                if (first) {
                    struct search_option so = alphabeta_search(&pos_cpy,
                                                  hist,
                                                  tt,
                                                  mailbox_cpy,
                                                  other_side(us),
                                                  depth - 1,
                                                  (Score16)(-beta),
                                                  (Score16)(-alpha)
                                                 #ifdef FEATURE_STOPPABLE_SEARCH
                                                  ,
                                                  searching
                                                 #endif
                                                  );
                    if (!so.init) {
                        hist->length = old_hist_len;
                        return so;
                    }
                    score = (Score16)-so.score;
                    first = false;
                } else {
                    struct search_option so;
                    assuming(alpha < beta);
                    g_pvs_probes += 1;
                    so = alphabeta_search(&pos_cpy,
                                          hist,
                                          tt,
                                          mailbox_cpy,
                                          other_side(us),
                                          depth - 1,
                                          (Score16)(-alpha - 1),
                                          (Score16)(-alpha)
                                         #ifdef FEATURE_STOPPABLE_SEARCH
                                          ,
                                          searching
                                         #endif
                    );
                    if (!so.init) {
                        hist->length = old_hist_len;
                        return so;
                    }
                    if ((Score16)-so.score > alpha) {
                        g_pvs_re_search += 1;

                        so = alphabeta_search(&pos_cpy,
                                 hist,
                                 tt,
                                 mailbox_cpy,
                                 other_side(us),
                                 depth - 1,
                                 (Score16)(-beta),
                                 (Score16)(-alpha)
                                #ifdef FEATURE_STOPPABLE_SEARCH
                                 ,
                                 searching
                                #endif
                        );
                        if (!so.init) {
                            hist->length = old_hist_len;
                            return so;
                        }
                    }
                    score = (Score16)-so.score;
                }
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

    if (IS_MOVE_NULL(best_move)) {
        return (struct search_option){ .init = true, .move = MOVE_NULL, .score = -(SCORE_CHECKMATE + depth) };
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

    assuming(out.init);
    return out;
}

static
struct search_result {struct move move; Score16 score;}
search(
          struct board* restrict b, 
          Side8 us,
          int8_t max_depth
         #ifdef FEATURE_STOPPABLE_SEARCH
          ,
          struct searching_flag const* restrict searching
         #endif
          )
{
    struct move moves[MOVE_MAX];
    size_t move_count = 0;

    all_pseudolegal_moves(&b->pos, MG_ALL, us, &move_count, moves);

    assuming(move_count);

    struct move best_move = moves[0];

    g_ab_node_volume = 0;
    g_pvs_re_search = 0;
    g_pvs_probes = 0;

    Score16 score = 0;

    for (int8_t d = 1;
         d <= max_depth
#ifdef FEATURE_STOPPABLE_SEARCH
         && searching_still(searching)
#endif
         ;
         ++d)
    {
        Score16 window;

        if (d == 1) {
            window = SCORE_INF;
        } else {
            window = 20 + 8*d;
        }
#ifdef FEATURE_USE_PRINTF
        fprintf(stderr, "depth: %hhd - window %hd\n", d, window);
#endif

        while (1) {
            int al = (int)score - (int)window;
            int bt = (int)score + (int)window;

            if (al < -SCORE_INF) al = -SCORE_INF;
            if (bt >  SCORE_INF) bt =  SCORE_INF;

            Score16 alpha = (Score16)al;
            Score16 beta = (Score16)bt;

            struct search_option so =
                alphabeta_search(&b->pos, &b->hist, &b->tt, b->mailbox, us, d, alpha, beta
#ifdef FEATURE_STOPPABLE_SEARCH
                        , searching
#endif
                        );

#ifdef FEATURE_STOPPABLE_SEARCH
            if (!searching_still(searching)) goto stop_search;
#endif

            if (IS_MOVE_NULL(so.move)) goto stop_search;

            if (so.score >= alpha && so.score <= beta) {
                score = so.score;
                best_move = so.move;
                break;
            }

            if (window < SCORE_INF/2) {
#ifdef FEATURE_USE_PRINTF
                fprintf(stderr, "depth: %hhd - expanding window to %hd\n", d, window);
#endif
                window *= 2;
            } else {
                window = SCORE_INF;
            }
        }
    }

stop_search:

#ifdef FEATURE_USE_PRINTF
    fprintf(stderr, "nodes searched:  %'llu\n", g_ab_node_volume);
    fprintf(stderr, "pvs re-searches: %'llu\n", g_pvs_re_search);
    fprintf(stderr, "pvs probes:      %'llu\n", g_pvs_probes);
#endif

    return (struct search_result){.move = best_move, .score = score};
}


