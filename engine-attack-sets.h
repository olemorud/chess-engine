#pragma once

#include "engine-types.h"
#include "engine-board.h"

/* `struct magic` is used by magic bitboard lookups, see
 * rook_attacks_from_index and bishop_attacks_from_index */
struct magic {
    Bb64 mask;
    Bb64 magic;
};

static Bb64 cardinals_from_index(Sq8 p)
{
    return (FILE_MASK(sq_to_file(p)) | RANK_MASK(sq_to_rank(p)));
}

static Bb64 diagonals_from_index(Sq8 sq)
{
#ifdef CODEGEN
    enum rank_index const rank = sq_to_rank(sq);
    enum file_index const file = sq_to_file(sq);

    Bb64 mask = 0ULL;

    _Static_assert(((enum rank_index)-1) > 0, "rank_index must be unsigned");
    _Static_assert(((enum file_index)-1) > 0, "file_index must be unsigned");

    enum rank_index r;
    enum file_index f;
    for (r = rank+1, f = file+1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         ++r, ++f)
    {
        mask |= MASK_FROM_RF(r, f);
    }

    for (r = rank+1, f = file-1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         ++r, --f)
    {
        mask |= MASK_FROM_RF(r, f);
    }

    for (r = rank-1, f = file+1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         --r, ++f)
    {
        mask |= MASK_FROM_RF(r, f);
    }

    for (r = rank-1, f = file-1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         --r, --f)
    {
        mask |= MASK_FROM_RF(r, f);
    }
    return mask;

#else
    #if ! __has_include("diagonals.h")
        #error "compile with -DCODEGEN and run once to generate required header files"
    #endif
    #include "diagonals.h" /* defines static Bb64 diagonals[SQ_COUNT]; */
    return diagonals[sq];
#endif
}



/* PIECE ATTACKS
 * ==================
 *
 * All piece attack functions rely on the caller masking their own pieces.
 * e.g. `knight_attacks(knights) & ~our_occupied`
 * */

static Bb64 knight_attacks(Bb64 p)
{
    Bb64 const l1 = MASK_SHIFT_EAST_1(p /*& ~FILE_MASK_H*/);
    Bb64 const l2 = MASK_SHIFT_EAST_2(p /*& ~(FILE_MASK_G | FILE_MASK_H)*/);
    Bb64 const r1 = MASK_SHIFT_WEST_1 (p /*& ~FILE_MASK_A*/);
    Bb64 const r2 = MASK_SHIFT_WEST_2 (p /*& ~(FILE_MASK_A | FILE_MASK_B)*/);
    Bb64 const h1 = l1 | r1;
    Bb64 const h2 = l2 | r2;
    return MASK_SHIFT_NORTH_2(h1)
         | MASK_SHIFT_SOUTH_2(h1)
         | MASK_SHIFT_NORTH_1(h2)
         | MASK_SHIFT_SOUTH_1(h2);
}

static Bb64 knight_attacks_from_index(Sq8 sq)
{
    return knight_attacks(MASK_FROM_SQ(sq));
}

static Bb64 rook_attacks_from_index(Sq8 sq, Bb64 occ)
{
#ifdef CODEGEN
    enum rank_index const rank = sq_to_rank(sq);
    enum file_index const file = sq_to_file(sq);

    Bb64 atk = 0ULL;

    /* following loops assuming rank and file types are unsigned, which relies on C23 enums */
    _Static_assert(((enum rank_index)-1) > 0, "rank_index must be unsigned");
    _Static_assert(((enum file_index)-1) > 0, "file_index must be unsigned");

    for (enum rank_index walk_rank = rank+1; walk_rank <= RANK_INDEX_8; ++walk_rank) {
        Bb64 const sq = MASK_FROM_RF(walk_rank, file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (enum rank_index walk_rank = rank-1; walk_rank <= RANK_INDEX_8; --walk_rank) {
        Bb64 const sq = MASK_FROM_RF(walk_rank, file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (enum file_index walk_file = file+1; walk_file <= FILE_INDEX_H; ++walk_file) {
        Bb64 const sq = MASK_FROM_RF(rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (enum file_index walk_file = file-1; walk_file <= FILE_INDEX_H; --walk_file) {
        Bb64 const sq = MASK_FROM_RF(rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }

    return atk;

#else
    #if ! __has_include("mbb_rook.h")
        #error "compile with -DCODEGEN and run once to generate required header files"
    #else

    /* defines
       - `mbb_rook[SQ_COUNT]`
       - `Bb64 rook_attacks[SQ_COUNT][1<<12ULL] */
    #include "mbb_rook.h"

    struct magic const m = mbb_rook[sq];
    occ &= m.mask;
    occ *= m.magic;
    occ >>= (64ULL-12ULL);
    assuming(rook_attacks[sq][occ] != MASK_FROM_SQ(sq));
    return rook_attacks[sq][occ];

    #endif
#endif
}

static Bb64 rook_attacks(Bb64 p, Bb64 occ)
{
    Bb64 b = 0ULL;
    while (p) {
        Sq8 const lsb = bitboard_pop_lsb(&p);
        b |= rook_attacks_from_index(lsb, occ);
    }
    return b;
}

static Bb64 bishop_attacks_from_index(Sq8 sq, Bb64 occ)
{
#ifdef CODEGEN
    enum rank_index const rank = sq_to_rank(sq);
    enum file_index const file = sq_to_file(sq);

    Bb64 atk = 0ULL;

    /* following loops assuming rank and file types are unsigned, which relies on C23 enums */
    _Static_assert(((enum rank_index)-1) > 0, "rank_index must be unsigned");
    _Static_assert(((enum file_index)-1) > 0, "file_index must be unsigned");

    enum rank_index walk_rank;
    enum file_index walk_file;
    for (walk_rank = rank+1, walk_file = file+1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            ++walk_rank, ++walk_file) {
        Bb64 const sq = MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (walk_rank = rank+1, walk_file = file-1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            ++walk_rank, --walk_file) {
        Bb64 const sq = MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (walk_rank = rank-1, walk_file = file+1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            --walk_rank, ++walk_file) {
        Bb64 const sq = MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (walk_rank = rank-1, walk_file = file-1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            --walk_rank, --walk_file) {
        Bb64 const sq = MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }

    return atk;
#else
    #if ! __has_include("mbb_bishop.h")
        #error "compile with -DCODEGEN and run once to generate required header files"
    #else

/* defines
   - `mbb_bishop[SQ_COUNT]`
   - `Bb64 bishop_attacks[SQ_COUNT][1<<9ULL] */
#include "mbb_bishop.h"
    assuming(sq < SQ_COUNT);
    struct magic const m = mbb_bishop[sq];
    occ &= m.mask;
    occ *= m.magic;
    occ >>= (64ULL-9ULL);
    return bishop_attacks[sq][occ];
    #endif
#endif
}

static Bb64 bishop_attacks(Bb64 p, Bb64 occ)
{
    Bb64 b = 0ULL;
    while (p) {
        Sq8 const lsb = bitboard_pop_lsb(&p);
        b |= bishop_attacks_from_index(lsb, occ);
    }
    return b;
}

static Bb64 queen_attacks_from_index(Sq8 sq, Bb64 occ)
{
    return bishop_attacks_from_index(sq, occ) | rook_attacks_from_index(sq, occ);
}

static Bb64 queen_attacks(Bb64 p, Bb64 occ)
{
    Bb64 b = 0ULL;
    while (p) {
        Sq8 const lsb = bitboard_pop_lsb(&p);
        b |= queen_attacks_from_index(lsb, occ);
    }
    return b;
}

#define DEFINE_PAWN_MOVE_FUNCTIONS(identifier, side_enum, direction, starting_rank, next_rank) \
static inline Bb64 pawn_moves_##identifier(Bb64 p, Bb64 empty)\
{\
    Bb64 const single_push = MASK_SHIFT_##direction##_1(p) & empty;\
    Bb64 const double_push =\
        MASK_SHIFT_##direction##_1(single_push & next_rank) & empty;\
    return (single_push | double_push);\
}\
\
static inline Bb64 pawn_moves_##identifier##_from_index(Sq8 sq, Bb64 empty)\
{\
    return pawn_moves_##identifier(MASK_FROM_SQ(sq), empty);\
}\
\
static inline Bb64 pawn_single_push_##identifier(Bb64 p, Bb64 empty)\
{\
    return MASK_SHIFT_##direction##_1(p) & empty;\
}\
\
static inline Bb64 pawn_double_push_##identifier(Bb64 p, Bb64 empty)\
{\
    Bb64 const once  = MASK_SHIFT_##direction##_1(p) & empty;\
    Bb64 const twice = MASK_SHIFT_##direction##_1(once) & empty;\
    return twice;\
}\
\
static inline Bb64 pawn_attacks_right_##identifier(Bb64 p)\
{\
    Bb64 const up = MASK_SHIFT_##direction##_1(p);\
    return MASK_SHIFT_EAST_1(up);\
}\
\
static inline Bb64 pawn_attacks_left_##identifier(Bb64 p)\
{\
    Bb64 const up = MASK_SHIFT_##direction##_1(p);\
    return MASK_SHIFT_WEST_1(up);\
}\
\
static inline Bb64 pawn_attacks_##identifier(Bb64 p)\
{\
    Bb64 const up = MASK_SHIFT_##direction##_1(p);\
    Bb64 const captures_right = MASK_SHIFT_EAST_1(up);\
    Bb64 const captures_left  = MASK_SHIFT_WEST_1(up);\
\
    return (captures_right | captures_left);\
}\
\
static inline Bb64 pawn_attacks_##identifier##_from_index(Sq8 sq)\
{\
    return pawn_attacks_##identifier(MASK_FROM_SQ(sq));\
}

DEFINE_PAWN_MOVE_FUNCTIONS(white, SIDE_WHITE, NORTH, RANK_MASK_2, RANK_MASK_3)
DEFINE_PAWN_MOVE_FUNCTIONS(black, SIDE_BLACK, SOUTH, RANK_MASK_7, RANK_MASK_6)

/* temporary solution, annoying branch */
static inline Bb64 pawn_moves_dispatch(Bb64 p, Bb64 empty, Side8 attacker)
{
    if (attacker == SIDE_WHITE) {
        return pawn_moves_white(p, empty);
    } else {
        return pawn_moves_black(p, empty);
    }
}

/* temporary solution, annoying branch */
static inline Bb64 pawn_moves_from_index_dispatch(Sq8 sq, Bb64 empty, Side8 attacker)
{
    if (attacker == SIDE_WHITE) {
        return pawn_moves_white_from_index(sq, empty);
    } else {
        return pawn_moves_black_from_index(sq, empty);
    }
}

/* temporary solution, annoying branch */
static inline Bb64 pawn_attacks_from_index_dispatch(Sq8 sq, Side8 attacker)
{
    if (attacker == SIDE_WHITE) {
        return pawn_attacks_white_from_index(sq);
    } else {
        return pawn_attacks_black_from_index(sq);
    }
}

static inline Bb64 pawn_attacks_dispatch(Bb64 b, Side8 attacker)
{
    if (attacker == SIDE_WHITE) {
        return pawn_attacks_white(b);
    } else {
        return pawn_attacks_black(b);
    }
}

static Bb64 king_attacks(Bb64 sq)
{
    /* potential untested improvements:
     * - lookup table
     * - union of three files, three ranks and ~sq */
    Bb64 const n = MASK_SHIFT_NORTH_1(sq);
    Bb64 const s = MASK_SHIFT_SOUTH_1(sq);
    Bb64 const w = MASK_SHIFT_WEST_1(sq);
    Bb64 const e = MASK_SHIFT_EAST_1(sq);

    Bb64 const nw = MASK_SHIFT_WEST_1(n);
    Bb64 const ne = MASK_SHIFT_EAST_1(n);
    Bb64 const sw = MASK_SHIFT_WEST_1(s);
    Bb64 const se = MASK_SHIFT_EAST_1(s);

    return (n | s | w | e | nw | ne | sw | se);
}

static Bb64 king_attacks_from_index(Sq8 sq)
{
    return king_attacks(MASK_FROM_SQ(sq));
}

static Bb64 non_pawn_piece_attacks(Piece8 piece, Bb64 p, Bb64 occ)
{
    assuming(piece != PIECE_PAWN);

    switch (piece) {
        case PIECE_KNIGHT: {
            return knight_attacks(p);
        } break;

        case PIECE_BISHOP: {
            return bishop_attacks(p, occ);
        } break;

        case PIECE_ROOK: {
            return rook_attacks(p, occ);
        } break;

        case PIECE_QUEEN: {
            return queen_attacks(p, occ);
        } break;

        case PIECE_KING: {
            return king_attacks(p);
        } break;

        default: {
            assuming(false);
        } break;
    }
}

static Bb64 non_pawn_piece_attacks_from_index(Piece8 piece, Sq8 sq, Bb64 occ)
{
    assuming(piece != PIECE_PAWN);

    switch (piece) {
        case PIECE_KNIGHT: {
            return knight_attacks_from_index(sq);
        } break;

        case PIECE_BISHOP: {
            return bishop_attacks_from_index(sq, occ);
        } break;

        case PIECE_ROOK: {
            return rook_attacks_from_index(sq, occ);
        } break;

        case PIECE_QUEEN: {
            return queen_attacks_from_index(sq, occ);
        } break;

        case PIECE_KING: {
            return king_attacks_from_index(sq);
        } break;

        default: {
            assuming(false);
        } break;
    }
}

static Bb64 between_mask(Sq8 a, Sq8 b)
{
#ifdef CODEGEN
    enum rank_index const ra = sq_to_rank(a);
    enum file_index const fa = sq_to_file(a);
    enum rank_index const rb = sq_to_rank(b);
    enum file_index const fb = sq_to_file(b);

    /* directional differences */
    int const dr = (int)rb - (int)ra;
    int const df = (int)fb - (int)fa;

    int step_r = 0;
    int step_f = 0;

    if (df == 0 && dr != 0) {
        step_r = (dr > 0) ? 1 : -1;
        step_f = 0;
    }
    else if (dr == 0 && df != 0) {
        step_r = 0;
        step_f = (df > 0) ? 1 : -1;
    }
    else if (dr != 0 && (dr > 0 ? dr : -dr) == (df > 0 ? df : -df)) {
        step_r = (dr > 0) ? 1 : -1;
        step_f = (df > 0) ? 1 : -1;
    }
    else {
        return 0ULL;
    }

    Bb64 mask = 0ULL;

    enum rank_index r = (enum rank_index)((int)ra + step_r);
    enum file_index f = (enum file_index)((int)fa + step_f);

    while (r != rb || f != fb) {
        mask |= MASK_FROM_RF(r, f);
        r = (enum rank_index)((int)r + step_r);
        f = (enum file_index)((int)f + step_f);
    }

    return mask;
#else
    #if ! __has_include("between_lookup.h")
        #error "compile codegen.c and run once to generate required header files"
    #else

    /* defines static between_lookup[sq8_COUNT][sq8_COUNT] */
    #include "between_lookup.h"
    return between_lookup[a][b];
    #endif
#endif
}

static Bb64 attacks_to(struct pos const* pos,
                       Bb64              targets,
                       Bb64              pretend_occupied,
                       Bb64              pretend_empty)
{
    Bb64 const occ_orig = pos->occupied[SIDE_WHITE] | pos->occupied[SIDE_BLACK];
    Bb64 const occ = (occ_orig & ~pretend_empty) | pretend_occupied;

    Bb64 const* pw = pos->pieces[SIDE_WHITE];
    Bb64 const* pb = pos->pieces[SIDE_BLACK];

    Bb64 const wps = pw[PIECE_PAWN];

    Bb64 const bps = pb[PIECE_PAWN];

    Bb64 const ns = (pw[PIECE_KNIGHT]
                  |  pb[PIECE_KNIGHT])
                  ;
    Bb64 const ks = (pw[PIECE_KING]
                  |  pb[PIECE_KING])
                  ;
    Bb64 const qs = (pw[PIECE_QUEEN]
                  |  pb[PIECE_QUEEN])
                  ;
    Bb64 const qrs = (pw[PIECE_ROOK]
                   |  pb[PIECE_ROOK]
                   |  qs)
                   ;
    Bb64 const qbs = (pw[PIECE_BISHOP]
                   |  pb[PIECE_BISHOP]
                   |  qs)
                   ;

    return ((bps & pawn_attacks_white(targets))
          | (wps & pawn_attacks_black(targets))
          | (ns  & knight_attacks(targets))
          | (qbs & bishop_attacks(targets, occ))
          | (qrs & rook_attacks(targets, occ))
          | (ks  & king_attacks(targets)))
          & ~pretend_occupied;
}

static
Bb64 checkers(struct pos const* pos, Side8 us)
{
    /* TODO: specialize */
    return attacks_to(pos, pos->pieces[us][PIECE_KING], 0ULL, 0ULL) & ~pos->occupied[us];
}

static
Bb64 pinning_lines_to_target(struct pos const* pos, Side8 us, Sq8 tsq)
{
    Side8 const them = other_side(us);
    Bb64 const our_occ = pos->occupied[us];
    Bb64 const their_occ = pos->occupied[them];

    Bb64 const* p = pos->pieces[them];
    Bb64 const bqs = p[PIECE_QUEEN] | p[PIECE_BISHOP];
    Bb64 const rqs = p[PIECE_QUEEN] | p[PIECE_ROOK];

    Bb64 pinners = (bqs & bishop_attacks_from_index(tsq, their_occ))
                     | (rqs & rook_attacks_from_index(tsq,   their_occ));

    Bb64 pinned = 0;
    while (pinners) {
        Sq8 const sq = bitboard_pop_lsb(&pinners);
        Bb64 const blockers = between_mask(tsq, sq) & our_occ;
        if (!bitboard_more_than_one(blockers)) {
            pinned |= blockers;
        }
    }

    return pinned;
}

static
Bb64 all_threats_from_side(struct pos const * pos, Side8 who)
{
    Bb64 const occ = pos->occupied[SIDE_WHITE] | pos->occupied[SIDE_BLACK];

    Bb64 const* p = pos->pieces[who];
    Bb64 t = 0ULL;
    t |= pawn_attacks_dispatch(p[PIECE_PAWN], who);
    t |= knight_attacks(p[PIECE_KNIGHT]);
    t |= bishop_attacks(p[PIECE_BISHOP], occ);
    t |= rook_attacks(p[PIECE_ROOK], occ);
    t |= queen_attacks(p[PIECE_QUEEN], occ);
    t |= king_attacks(p[PIECE_KING]);
    return t;
}

