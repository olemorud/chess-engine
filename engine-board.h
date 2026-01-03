#pragma once

#include "engine-types.h"
#include "engine-tt.h"

#include <stddef.h>
#include <stdint.h>

struct board {
    struct pos {
        Bb64  pieces[SIDE_COUNT][PIECE_COUNT];
        Side8 moving_side;
        bool  castling_illegal[SIDE_COUNT][CASTLE_COUNT];
        Bb64  ep_targets;
        Bb64  occupied[SIDE_COUNT];

        int halfmoves; /* FIXME: this duplicates the hist.length value */
        int fullmoves;
        uint64_t hash;
    } pos;

    struct tt tt;

    /* used for repeated board state detection only */
    struct history {
        struct pos items[64];
        size_t length;
    } hist;

    /* Used only for square->piece lookups, not central to logic.  Does not
     * store which side owns the square piece, and checking non-occupied
     * squares is undefined
     *
     * TODO: make mailbox smaller, only 3 bits are needed per piece
     * */
    Piece8 mailbox[SQ_COUNT];
};

#define BOARD_INIT (struct board) {                       \
    .pos = {                                              \
        .fullmoves = 1,                                   \
        .pieces = {                                       \
            [SIDE_WHITE] = {                              \
                [PIECE_PAWN]   = RANK_MASK_2,             \
                [PIECE_ROOK]   = SQMASK_A1 | SQMASK_H1,   \
                [PIECE_KNIGHT] = SQMASK_B1 | SQMASK_G1,   \
                [PIECE_BISHOP] = SQMASK_C1 | SQMASK_F1,   \
                [PIECE_QUEEN]  = SQMASK_D1,               \
                [PIECE_KING]   = SQMASK_E1,               \
            },                                            \
            [SIDE_BLACK] = {                              \
                [PIECE_PAWN]   = RANK_MASK_7,             \
                [PIECE_ROOK]   = SQMASK_A8 | SQMASK_H8,   \
                [PIECE_KNIGHT] = SQMASK_B8 | SQMASK_G8,   \
                [PIECE_BISHOP] = SQMASK_C8 | SQMASK_F8,   \
                [PIECE_QUEEN]  = SQMASK_D8,               \
                [PIECE_KING]   = SQMASK_E8,               \
            }                                             \
        },                                                \
        .occupied = {                                     \
            [SIDE_WHITE] =                                \
                RANK_MASK_2 | SQMASK_A1 | SQMASK_H1 |     \
                SQMASK_B1 | SQMASK_G1 | SQMASK_C1 |       \
                SQMASK_F1 | SQMASK_D1 | SQMASK_E1,        \
            [SIDE_BLACK] =                                \
                RANK_MASK_7 | SQMASK_A8 | SQMASK_H8 |     \
                SQMASK_B8 | SQMASK_G8| SQMASK_C8 |        \
                SQMASK_F8| SQMASK_D8| SQMASK_E8,          \
        },                                                \
        .hash = ~0ULL,                                    \
    },                                                    \
    .mailbox = {                                          \
        [SQ_A1] = PIECE_ROOK,                             \
        [SQ_B1] = PIECE_KNIGHT,                           \
        [SQ_C1] = PIECE_BISHOP,                           \
        [SQ_D1] = PIECE_QUEEN,                            \
        [SQ_E1] = PIECE_KING,                             \
        [SQ_F1] = PIECE_BISHOP,                           \
        [SQ_G1] = PIECE_KNIGHT,                           \
        [SQ_H1] = PIECE_ROOK,                             \
        [SQ_A2] = PIECE_PAWN,                             \
        [SQ_B2] = PIECE_PAWN,                             \
        [SQ_C2] = PIECE_PAWN,                             \
        [SQ_D2] = PIECE_PAWN,                             \
        [SQ_E2] = PIECE_PAWN,                             \
        [SQ_F2] = PIECE_PAWN,                             \
        [SQ_G2] = PIECE_PAWN,                             \
        [SQ_H2] = PIECE_PAWN,                             \
        [SQ_A7] = PIECE_PAWN,                             \
        [SQ_B7] = PIECE_PAWN,                             \
        [SQ_C7] = PIECE_PAWN,                             \
        [SQ_D7] = PIECE_PAWN,                             \
        [SQ_E7] = PIECE_PAWN,                             \
        [SQ_F7] = PIECE_PAWN,                             \
        [SQ_G7] = PIECE_PAWN,                             \
        [SQ_H7] = PIECE_PAWN,                             \
        [SQ_A8] = PIECE_ROOK,                             \
        [SQ_B8] = PIECE_KNIGHT,                           \
        [SQ_C8] = PIECE_BISHOP,                           \
        [SQ_D8] = PIECE_QUEEN,                            \
        [SQ_E8] = PIECE_KING,                             \
        [SQ_F8] = PIECE_BISHOP,                           \
        [SQ_G8] = PIECE_KNIGHT,                           \
        [SQ_H8] = PIECE_ROOK,                             \
    },                                                    \
    .hist = {0},                                          \
}

void board_init(struct board* b)
{
    if (b->pos.fullmoves == 0 && b->pos.hash == 0) {
        *b = BOARD_INIT;
    }
    if (b->tt.entries == NULL) {
        b->tt.entries = sys_mmap_anon_shared(TT_ENTRIES * sizeof b->tt.entries[0],
                                             SYS_PROT_READ | SYS_PROT_WRITE,
                                             SYS_MADV_RANDOM);
        if (b->tt.entries == NULL) {
            __builtin_trap();
        }
    }
}

static bool board_load_fen_unsafe(struct board* b, char const* fen_str)
{
    /* TODO: this function is not tested for malicious/corrupted inputs */
    struct {
        char const* buf;
        size_t i;
        size_t len;
    } fen = {
        .buf = fen_str,
        .i = 0,
        .len = strlen(fen_str),
    };
#define BUF_GETCHAR(x) (assuming(x.i < x.len), x.buf[x.i++])

    my_memset(&b->pos, 0, sizeof b->pos);
    b->pos.hash = ~0ULL;

    for (enum rank_index ri = RANK_INDEX_8; ri < RANK_INDEX_COUNT; --ri) {
        for (enum file_index fi = FILE_INDEX_BEGIN; fi < FILE_INDEX_COUNT; ++fi) {
            char const ch = BUF_GETCHAR(fen);
            if (my_isdigit(ch)) {
                if (ch == '0') {
                    __builtin_trap();
                }
                fi += ch - '0' - 1;
            } else {
                struct piece_side const p = piece_and_side_from_char[(uint8_t)ch];
                Bb64 const sq_mask = MASK_FROM_RF(ri, fi);
                b->pos.pieces[p.side][p.piece] |= sq_mask;
                b->pos.occupied[p.side] |= sq_mask;
                b->mailbox[SQ_FROM_RF(ri, fi)] = p.piece;
            }
        }
        (void)BUF_GETCHAR(fen); /* discard '/' or ' ' */
    }

    { /* active color */
        char const ch = BUF_GETCHAR(fen);
        if (ch == 'w') {
            b->pos.moving_side = SIDE_WHITE;
        } else if (ch == 'b') {
            b->pos.moving_side = SIDE_BLACK;
        } else {
            __builtin_trap();
        }
    }

    { /* castling */
        char ch = BUF_GETCHAR(fen);
        if (ch != ' ') {
            __builtin_trap();
        }
        b->pos.castling_illegal[SIDE_WHITE][CASTLE_KINGSIDE] = true;
        b->pos.castling_illegal[SIDE_WHITE][CASTLE_QUEENSIDE] = true;
        b->pos.castling_illegal[SIDE_BLACK][CASTLE_KINGSIDE] = true;
        b->pos.castling_illegal[SIDE_BLACK][CASTLE_QUEENSIDE] = true;
        do {
            ch = BUF_GETCHAR(fen);
            switch (ch) {
                case 'K': b->pos.castling_illegal[SIDE_WHITE][CASTLE_KINGSIDE] = false;  break;
                case 'Q': b->pos.castling_illegal[SIDE_WHITE][CASTLE_QUEENSIDE] = false; break;
                case 'k': b->pos.castling_illegal[SIDE_BLACK][CASTLE_KINGSIDE] = false;  break;
                case 'q': b->pos.castling_illegal[SIDE_BLACK][CASTLE_QUEENSIDE] = false; break;
                case ' ': break;
                case '-': break;
                default: {
                    __builtin_trap();
                } break;
            }
        } while (ch != ' ');
    }

    { /* en passent */
        char const ch = BUF_GETCHAR(fen);
        if (ch != '-') {
            __builtin_trap();
        }
    }

    { /* halfmoves */
        b->pos.halfmoves = 0;
        char ch = BUF_GETCHAR(fen);
        while (ch != ' ') {
            b->pos.halfmoves *= 10;
            b->pos.halfmoves += ch-'0';
            ch = BUF_GETCHAR(fen);
        }
    }

    { /* fullmoves */
        b->pos.fullmoves = 0;
        char ch = BUF_GETCHAR(fen);
        while (ch != ' ') {
            b->pos.fullmoves *= 10;
            b->pos.fullmoves += ch-'0';
            ch = BUF_GETCHAR(fen);
        }
    }
#undef BUF_GETCHAR

    return true;
}

enum move_result {
    MR_NORMAL,
    MR_CHECK,
    MR_REPEATS, /* means this board state has been observed before */
    MR_STALEMATE,
    MR_CHECKMATE,
};

/* does not check validity */
static enum move_result move_piece(struct pos* restrict     pos,
                                   struct history* restrict hist,
                                   Piece8                   mailbox[restrict static SQ_COUNT],
                                   struct move              move)
{
    Side8 const us   = pos->moving_side;
    Side8 const them = other_side(us);

    Piece8 const from_piece = mailbox[move.from];
    Piece8 const to_piece = mailbox[move.to];

    Bb64 const from_mask = MASK_FROM_SQ(move.from);
    Bb64 const to_mask   = MASK_FROM_SQ(move.to);

    Sq8 const krook       = (us == SIDE_WHITE) ? SQ_H1 : SQ_H8;
    Sq8 const qrook       = (us == SIDE_WHITE) ? SQ_A1 : SQ_A8;
    Sq8 const krook_to    = (us == SIDE_WHITE) ? SQ_F1 : SQ_F8;
    Sq8 const qrook_to    = (us == SIDE_WHITE) ? SQ_D1 : SQ_D8;
    Sq8 const ksq         = (us == SIDE_WHITE) ? SQ_E1 : SQ_E8;
    Sq8 const kcast_sq    = (us == SIDE_WHITE) ? SQ_G1 : SQ_G8;
    Sq8 const qcast_sq    = (us == SIDE_WHITE) ? SQ_C1 : SQ_C8;
    Sq8 const their_krook = (us != SIDE_WHITE) ? SQ_H1 : SQ_H8;
    Sq8 const their_qrook = (us != SIDE_WHITE) ? SQ_A1 : SQ_A8;

    #define POS_MOVE(side, piece, from, to) \
        do { \
            Bb64 const x = MASK_FROM_SQ(from) | MASK_FROM_SQ(to); \
            pos->pieces[side][piece] ^= x; \
            pos->occupied[side] ^= x; \
            pos->hash = tt_hash_update(pos->hash, from, side, piece); \
            pos->hash = tt_hash_update(pos->hash, to, side, piece); \
            mailbox[to] = piece; \
            if (piece == PIECE_PAWN) pos->halfmoves = 0; \
        } while (0)

    #define POS_REMOVE(owner, piece, at) \
        do { \
            Bb64 const x = MASK_FROM_SQ(at); \
            pos->pieces[owner][piece] &= ~x; \
            pos->occupied[owner] &= ~x; \
            pos->hash = tt_hash_update(pos->hash, at, owner, piece); \
            hist->length = 0; \
            pos->halfmoves = 0; \
        } while (0)

    #define POS_ADD(owner, piece, at) \
        do { \
            Bb64 const x = MASK_FROM_SQ(at); \
            pos->pieces[owner][piece] |= x; \
            pos->occupied[owner] |= x; \
            pos->hash = tt_hash_update(pos->hash, at, owner, piece); \
            mailbox[at] = piece; \
            pos->halfmoves = 0; \
            hist->length = 0; \
        } while (0)

    Bb64 const ep_targets_now = pos->ep_targets;
    if (ep_targets_now) {
        pos->hash = tt_hash_update_ep_targets(pos->hash, bitboard_lsb(ep_targets_now));
    }
    pos->ep_targets = 0ULL;

    /* castle kingside, legality is checked by the caller */
    /**/ if (from_piece == PIECE_KING && move.from == ksq && move.to == kcast_sq) {
        POS_MOVE(us, PIECE_KING, ksq, kcast_sq);
        POS_MOVE(us, PIECE_ROOK, krook, krook_to);
    }
    /* castle queenside, legality is checked by the caller */
    else if (from_piece == PIECE_KING && move.from == ksq && move.to == qcast_sq) {
        POS_MOVE(us, PIECE_KING, ksq, qcast_sq);
        POS_MOVE(us, PIECE_ROOK, qrook, qrook_to);
    }
    /* regular move / capture */
    else {
        POS_MOVE(us, from_piece, move.from, move.to);
        /* capture */
        if (to_mask & pos->occupied[them]) {
            POS_REMOVE(them, to_piece, move.to);
        }

        /* promote / ep */
        if (from_piece == PIECE_PAWN) {
            Bb64 const finishline = (us == SIDE_WHITE ? RANK_MASK_8 : RANK_MASK_1);

            /* en passent */
            /**/ if (to_mask & ep_targets_now) {
                Sq8 const ti =
                    (us == SIDE_WHITE)
                    ? SQ_SHIFT_SOUTH(move.to, 1)
                    : SQ_SHIFT_NORTH(move.to, 1);
                POS_REMOVE(them, PIECE_PAWN, ti);
            }
            /* pawn double push -> new ep_target */
            else if (us == SIDE_WHITE && (from_mask & RANK_MASK_2) && (to_mask & RANK_MASK_4)) {
                pos->ep_targets |= MASK_SHIFT_NORTH_1(from_mask);
                pos->hash = tt_hash_update_ep_targets(pos->hash, SQ_SHIFT_NORTH(move.from, 1));
            }
            else if (us == SIDE_BLACK && (from_mask & RANK_MASK_7) && (to_mask & RANK_MASK_5)) {
                pos->ep_targets |= MASK_SHIFT_SOUTH_1(from_mask);
                pos->hash = tt_hash_update_ep_targets(pos->hash, SQ_SHIFT_SOUTH(move.from, 1));
            }
            /* promote */
            else if (to_mask & finishline) {
                /* already moved to `move.to` */
                POS_REMOVE(us, PIECE_PAWN, move.to);
                POS_ADD(us, PIECE_QUEEN, move.to);
            }
        }
    }

    /* castling rights */
    if (!pos->castling_illegal[us][CASTLE_KINGSIDE]) {
        if (move.from == ksq || move.from == krook) {
            pos->castling_illegal[us][CASTLE_KINGSIDE] = true;
            pos->hash = tt_hash_update_castling_rights(pos->hash, us, CASTLE_KINGSIDE);
        }
    }
    if (!pos->castling_illegal[us][CASTLE_QUEENSIDE]) {
        if (move.from == ksq || move.from == qrook) {
            pos->castling_illegal[us][CASTLE_QUEENSIDE] = true;
            pos->hash = tt_hash_update_castling_rights(pos->hash, us, CASTLE_QUEENSIDE);
        }
    }

    if (move.to == their_krook) {
        if (!pos->castling_illegal[them][CASTLE_KINGSIDE]) {
            pos->castling_illegal[them][CASTLE_KINGSIDE] = true;
            pos->hash = tt_hash_update_castling_rights(pos->hash, them, CASTLE_KINGSIDE);
        }
    }
    if (move.to == their_qrook) {
        if (!pos->castling_illegal[them][CASTLE_QUEENSIDE]) {
            pos->castling_illegal[them][CASTLE_QUEENSIDE] = true;
            pos->hash = tt_hash_update_castling_rights(pos->hash, them, CASTLE_QUEENSIDE);
        }
    }

    pos->hash = tt_hash_switch_side(pos->hash);
    pos->moving_side = them;
    pos->fullmoves += (pos->moving_side == SIDE_BLACK);
    pos->halfmoves += 1;

    assuming(hist->length < 64);
    int repetitions = 0;
    for (size_t i = 0; i < hist->length; ++i) {
        _Static_assert(sizeof *pos == sizeof hist->items[i]);
        if (!my_memcmp(&hist->items[i].pieces, &pos->pieces, sizeof pos->pieces)
         && !my_memcmp(&hist->items[i].castling_illegal, &pos->castling_illegal, sizeof pos->castling_illegal)
         && hist->items[i].moving_side == pos->moving_side
         && hist->items[i].ep_targets == pos->ep_targets)
        {
            repetitions += 1;
        }
    }

    hist->items[hist->length++] = *pos;

    if (repetitions >= 3 || pos->halfmoves > 50) {
        return MR_STALEMATE;
    }
    else if (repetitions > 0) {
        return MR_REPEATS;
    }
    else if (pos->halfmoves > 50) {
        return MR_STALEMATE;
    }
#if 0
    else if (attacks_to(pos, pos->pieces[them][PIECE_KING], 0ULL, 0ULL)
            & ~pos->occupied[them]) {
        return MR_CHECK;
    }
#endif
    else {
        return MR_NORMAL;
    }

#undef POS_MOVE
#undef POS_ADD
#undef POS_REMOVE
}

static enum move_result board_move(struct board* b, struct move move)
{
    return move_piece(&b->pos,
                      &b->hist,
                      b->mailbox,
                      move);
}
