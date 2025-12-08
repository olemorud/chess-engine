#pragma once

#include <locale.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/* BIT MANIPULATION
 * ================ */

static inline uint64_t popcount(uint64_t n)
{
    // TODO: check of popcountll support  
    return (uint64_t)__builtin_popcountll(n);
}

static inline uint64_t ctz(uint64_t n)
{
    assert(n != 0);
    return __builtin_ctzll(n);
}

/* BITBOARD AND INDEX DATATYPES AND HELPERS
 * ========================================= */

typedef uint64_t bitboard;

#define BITBOARD(                \
        a8,b8,c8,d8,e8,f8,g8,h8, \
        a7,b7,c7,d7,e7,f7,g7,h7, \
        a6,b6,c6,d6,e6,f6,g6,h6, \
        a5,b5,c5,d5,e5,f5,g5,h5, \
        a4,b4,c4,d4,e4,f4,g4,h4, \
        a3,b3,c3,d3,e3,f3,g3,h3, \
        a2,b2,c2,d2,e2,f2,g2,h2, \
        a1,b1,c1,d1,e1,f1,g1,h1) \
(bitboard)\
0b##\
h8##g8##f8##e8##d8##c8##b8##a8##\
h7##g7##f7##e7##d7##c7##b7##a7##\
h6##g6##f6##e6##d6##c6##b6##a6##\
h5##g5##f5##e5##d5##c5##b5##a5##\
h4##g4##f4##e4##d4##c4##b4##a4##\
h3##g3##f3##e3##d3##c3##b3##a3##\
h2##g2##f2##e2##d2##c2##b2##a2##\
h1##g1##f1##e1##d1##c1##b1##a1##\
ULL

#define BITBOARD_FMT PRIu64
#define BITBOARD_FMT_X PRIx64


typedef uint64_t index;
#define INDEX(n) n##ULL
#define INDEX_FMT PRIu64

#define RANK_SHIFT_UP(b, n)    ((b) << (index)(n)*8ULL)
#define RANK_SHIFT_DOWN(b, n)  ((b) >> (index)(n)*8ULL)
#define FILE_SHIFT_LEFT(b, n)  ((b) >> (index)(n)*1ULL)
#define FILE_SHIFT_RIGHT(b, n) ((b) << (index)(n)*1ULL)

#define FILE_MASK(n) FILE_SHIFT_RIGHT((index)0x0101010101010101ULL, n)
#define RANK_MASK(n) RANK_SHIFT_UP((index)0x00000000000000FFULL, n)

#define INDEX_FROM_RF(rank, file) ((index)(8ULL*(index)(rank) + (index)(file)))
#define SQUARE_MASK_FROM_RF(rank, file) ((index)1ULL << INDEX_FROM_RF(rank,file))

#define SQUARE_MASK_FROM_INDEX(idx) (1ULL<<(idx))

enum file_index : index {
    FILE_INDEX_BEGIN,
    FILE_INDEX_A = FILE_INDEX_BEGIN,
    FILE_INDEX_B,
    FILE_INDEX_C,
    FILE_INDEX_D,
    FILE_INDEX_E,
    FILE_INDEX_F,
    FILE_INDEX_G,
    FILE_INDEX_H,
    FILE_INDEX_COUNT,
};

enum rank_index : index {
    RANK_INDEX_BEGIN,
    RANK_INDEX_1 = RANK_INDEX_BEGIN,
    RANK_INDEX_2,
    RANK_INDEX_3,
    RANK_INDEX_4,
    RANK_INDEX_5,
    RANK_INDEX_6,
    RANK_INDEX_7,
    RANK_INDEX_8,
    RANK_INDEX_COUNT,
};

enum : bitboard {
    FILEMASK_A = FILE_MASK(FILE_INDEX_A),
    FILEMASK_B = FILE_MASK(FILE_INDEX_B),
    FILEMASK_C = FILE_MASK(FILE_INDEX_C),
    FILEMASK_D = FILE_MASK(FILE_INDEX_D),
    FILEMASK_E = FILE_MASK(FILE_INDEX_E),
    FILEMASK_F = FILE_MASK(FILE_INDEX_F),
    FILEMASK_G = FILE_MASK(FILE_INDEX_G),
    FILEMASK_H = FILE_MASK(FILE_INDEX_H),

    RANKMASK_1 = RANK_MASK(RANK_INDEX_1),
    RANKMASK_2 = RANK_MASK(RANK_INDEX_2),
    RANKMASK_3 = RANK_MASK(RANK_INDEX_3),
    RANKMASK_4 = RANK_MASK(RANK_INDEX_4),
    RANKMASK_5 = RANK_MASK(RANK_INDEX_5),
    RANKMASK_6 = RANK_MASK(RANK_INDEX_6),
    RANKMASK_7 = RANK_MASK(RANK_INDEX_7),
    RANKMASK_8 = RANK_MASK(RANK_INDEX_8),

    FILE_SHIFT_RIGHT_GUARD_0 = ~0ULL,
    FILE_SHIFT_RIGHT_GUARD_1 = ~FILEMASK_H,
    FILE_SHIFT_RIGHT_GUARD_2 = ~(FILEMASK_G | FILEMASK_H),
    FILE_SHIFT_RIGHT_GUARD_3 = ~(FILEMASK_F | FILEMASK_G | FILEMASK_H),
    FILE_SHIFT_RIGHT_GUARD_4 = ~(FILEMASK_E | FILEMASK_F | FILEMASK_G | FILEMASK_H),
    FILE_SHIFT_RIGHT_GUARD_5 = ~(FILEMASK_D | FILEMASK_E | FILEMASK_F | FILEMASK_G | FILEMASK_H),
    FILE_SHIFT_RIGHT_GUARD_6 = ~(FILEMASK_C | FILEMASK_D | FILEMASK_E | FILEMASK_F | FILEMASK_G | FILEMASK_H),
    FILE_SHIFT_RIGHT_GUARD_7 = ~(FILEMASK_B | FILEMASK_C | FILEMASK_D | FILEMASK_E | FILEMASK_F | FILEMASK_G | FILEMASK_H),

    FILE_SHIFT_LEFT_GUARD_0  = ~0ULL,
    FILE_SHIFT_LEFT_GUARD_1  = ~FILEMASK_A,
    FILE_SHIFT_LEFT_GUARD_2  = ~(FILEMASK_A | FILEMASK_B),
    FILE_SHIFT_LEFT_GUARD_3  = ~(FILEMASK_A | FILEMASK_B | FILEMASK_C),
    FILE_SHIFT_LEFT_GUARD_4  = ~(FILEMASK_A | FILEMASK_B | FILEMASK_C | FILEMASK_D),
    FILE_SHIFT_LEFT_GUARD_5  = ~(FILEMASK_A | FILEMASK_B | FILEMASK_C | FILEMASK_D | FILEMASK_E),
    FILE_SHIFT_LEFT_GUARD_6  = ~(FILEMASK_A | FILEMASK_B | FILEMASK_C | FILEMASK_D | FILEMASK_E | FILEMASK_F),
    FILE_SHIFT_LEFT_GUARD_7  = ~(FILEMASK_A | FILEMASK_B | FILEMASK_C | FILEMASK_D | FILEMASK_E | FILEMASK_F | FILEMASK_G),
};


#define STR(x) #x

#define SQUARE_MASK_PREFIX SQUARE_MASK_
#define SQUARES_LIST_BEGIN \
    X(A, 1)
#define SQUARES_LIST \
    X(A, 1) \
    X(A, 2) \
    X(A, 3) \
    X(A, 4) \
    X(A, 5) \
    X(A, 6) \
    X(A, 7) \
    X(A, 8) \
    X(B, 1) \
    X(B, 2) \
    X(B, 3) \
    X(B, 4) \
    X(B, 5) \
    X(B, 6) \
    X(B, 7) \
    X(B, 8) \
    X(C, 1) \
    X(C, 2) \
    X(C, 3) \
    X(C, 4) \
    X(C, 5) \
    X(C, 6) \
    X(C, 7) \
    X(C, 8) \
    X(D, 1) \
    X(D, 2) \
    X(D, 3) \
    X(D, 4) \
    X(D, 5) \
    X(D, 6) \
    X(D, 7) \
    X(D, 8) \
    X(E, 1) \
    X(E, 2) \
    X(E, 3) \
    X(E, 4) \
    X(E, 5) \
    X(E, 6) \
    X(E, 7) \
    X(E, 8) \
    X(F, 1) \
    X(F, 2) \
    X(F, 3) \
    X(F, 4) \
    X(F, 5) \
    X(F, 6) \
    X(F, 7) \
    X(F, 8) \
    X(G, 1) \
    X(G, 2) \
    X(G, 3) \
    X(G, 4) \
    X(G, 5) \
    X(G, 6) \
    X(G, 7) \
    X(G, 8) \
    X(H, 1) \
    X(H, 2) \
    X(H, 3) \
    X(H, 4) \
    X(H, 5) \
    X(H, 6) \
    X(H, 7) \
    X(H, 8) \

/* define mask constants: SQUARE_MASK_A1, SQUARE_MASK_A2, ..., SQUARE_MASK_H8 */
enum : bitboard {
    #define X(file, rank) SQUARE_MASK_##file##rank = SQUARE_MASK_FROM_RF(RANK_INDEX_##rank, FILE_INDEX_##file),
    SQUARES_LIST
    #undef X
};

enum square_index : bitboard {
    #define X(file, rank) SQUARE_INDEX_##file##rank = INDEX_FROM_RF(RANK_INDEX_##rank, FILE_INDEX_##file),
    SQUARES_LIST
    SQUARE_INDEX_COUNT,
    #undef X
    /* define iterator begin enum */
    #define X(file, rank) SQUARE_INDEX_BEGIN = SQUARE_INDEX_##file##rank,
    SQUARES_LIST_BEGIN
    #undef X
};

const char* square_index_display[SQUARE_INDEX_COUNT] = {
    #define X(file, rank) \
    [SQUARE_INDEX_##file##rank] = STR(file##rank),
    SQUARES_LIST
    #undef X
};

const char* square_index_str[SQUARE_INDEX_COUNT] = {
    #define X(file, rank) \
    [SQUARE_INDEX_##file##rank] = STR(SQUARE_INDEX_##file##rank),
    SQUARES_LIST
    #undef X
};

#define _FILE_SHIFT_RIGHT_GUARDED(b, n) \
    ((((bitboard)(b)) & FILE_SHIFT_RIGHT_GUARD_##n) << ((index)(n) * 1ULL))

#define _FILE_SHIFT_LEFT_GUARDED(b, n)  \
    ((((bitboard)(b)) & FILE_SHIFT_LEFT_GUARD_##n) >> ((index)(n) * 1ULL))

/* FILE_SHIFT_RIGHT_n shifts the bitboard right by n ranks without wrapping */
#define FILE_SHIFT_RIGHT_1(b) _FILE_SHIFT_RIGHT_GUARDED(b, 1)
#define FILE_SHIFT_RIGHT_2(b) _FILE_SHIFT_RIGHT_GUARDED(b, 2)
#define FILE_SHIFT_RIGHT_3(b) _FILE_SHIFT_RIGHT_GUARDED(b, 3)
#define FILE_SHIFT_RIGHT_4(b) _FILE_SHIFT_RIGHT_GUARDED(b, 4)
#define FILE_SHIFT_RIGHT_5(b) _FILE_SHIFT_RIGHT_GUARDED(b, 5)
#define FILE_SHIFT_RIGHT_6(b) _FILE_SHIFT_RIGHT_GUARDED(b, 6)
#define FILE_SHIFT_RIGHT_7(b) _FILE_SHIFT_RIGHT_GUARDED(b, 7)
#define FILE_SHIFT_RIGHT_8(b) _FILE_SHIFT_RIGHT_GUARDED(b, 8)

/* FILE_SHIFT_LEFT_n shifts the bitboard left by n ranks without wrapping */
#define FILE_SHIFT_LEFT_1(b) _FILE_SHIFT_LEFT_GUARDED(b, 1)
#define FILE_SHIFT_LEFT_2(b) _FILE_SHIFT_LEFT_GUARDED(b, 2)
#define FILE_SHIFT_LEFT_3(b) _FILE_SHIFT_LEFT_GUARDED(b, 3)
#define FILE_SHIFT_LEFT_4(b) _FILE_SHIFT_LEFT_GUARDED(b, 4)
#define FILE_SHIFT_LEFT_5(b) _FILE_SHIFT_LEFT_GUARDED(b, 5)
#define FILE_SHIFT_LEFT_6(b) _FILE_SHIFT_LEFT_GUARDED(b, 6)
#define FILE_SHIFT_LEFT_7(b) _FILE_SHIFT_LEFT_GUARDED(b, 7)
#define FILE_SHIFT_LEFT_8(b) _FILE_SHIFT_LEFT_GUARDED(b, 8)

/* RANK_SHIFT_UP_n shifts the bitboard up by n ranks without wrapping */
#define RANK_SHIFT_UP_1(b) RANK_SHIFT_UP(b, 1)
#define RANK_SHIFT_UP_2(b) RANK_SHIFT_UP(b, 2)
#define RANK_SHIFT_UP_3(b) RANK_SHIFT_UP(b, 3)
#define RANK_SHIFT_UP_4(b) RANK_SHIFT_UP(b, 4)
#define RANK_SHIFT_UP_5(b) RANK_SHIFT_UP(b, 5)
#define RANK_SHIFT_UP_6(b) RANK_SHIFT_UP(b, 6)
#define RANK_SHIFT_UP_7(b) RANK_SHIFT_UP(b, 7)
#define RANK_SHIFT_UP_8(b) RANK_SHIFT_UP(b, 8)

/* RANK_SHIFT_DOWN_n shifts the bitboard down by n ranks without wrapping */
#define RANK_SHIFT_DOWN_1(b) RANK_SHIFT_DOWN(b, 1)
#define RANK_SHIFT_DOWN_2(b) RANK_SHIFT_DOWN(b, 2)
#define RANK_SHIFT_DOWN_3(b) RANK_SHIFT_DOWN(b, 3)
#define RANK_SHIFT_DOWN_4(b) RANK_SHIFT_DOWN(b, 4)
#define RANK_SHIFT_DOWN_5(b) RANK_SHIFT_DOWN(b, 5)
#define RANK_SHIFT_DOWN_6(b) RANK_SHIFT_DOWN(b, 6)
#define RANK_SHIFT_DOWN_7(b) RANK_SHIFT_DOWN(b, 7)
#define RANK_SHIFT_DOWN_8(b) RANK_SHIFT_DOWN(b, 8)

/* MISC 
 * ============*/
/* TODO: Put these struct definition in a sensible category */

/* `struct magic` is used by magic bitboard lookups, see
 * rook_attacks_from_index and bishop_attacks_from_index */
struct magic {
    bitboard mask;
    bitboard magic;
};

/* BITBOARD PRIMITIVES
 * ======================== */

static index bitboard_lsb(bitboard p)
{
    return __builtin_ffsll(p);
}

static uint64_t bitboard_popcount(bitboard b)
{
    return (uint64_t)__builtin_popcountll(b);
}

static inline index index_to_file(enum square_index p)
{
    return p % 8ULL;
}

static inline index index_to_rank(enum square_index p)
{
    return p / 8ULL;
}

static bitboard cardinals_from_index(enum square_index p)
{
    /* might benefit from a lookup table */
    return (FILE_MASK(index_to_file(p)) | RANK_MASK(index_to_rank(p)))
        & ~SQUARE_MASK_FROM_INDEX(p);
}

static bitboard diagonals_from_index(enum square_index sq)
{
    const enum rank_index rank = index_to_rank(sq);
    const enum file_index file = index_to_file(sq);

    bitboard mask = 0ULL;

    /* Ensure signed-underflow detection rules match your style */
    _Static_assert(((enum rank_index)0) - ((enum rank_index)1) > ((enum rank_index)0));
    _Static_assert(((enum file_index)0) - ((enum file_index)1) > ((enum file_index)0));

    enum rank_index r;
    enum file_index f;
    for (r = rank+1, f = file+1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         ++r, ++f)
    {
        mask |= SQUARE_MASK_FROM_RF(r, f);
    }

    /* NW (rank+1, file-1) */
    for (r = rank+1, f = file-1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         ++r, --f)
    {
        mask |= SQUARE_MASK_FROM_RF(r, f);
    }

    /* SE (rank-1, file+1) */
    for (r = rank-1, f = file+1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         --r, ++f)
    {
        mask |= SQUARE_MASK_FROM_RF(r, f);
    }

    /* SW (rank-1, file-1) */
    for (r = rank-1, f = file-1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         --r, --f)
    {
        mask |= SQUARE_MASK_FROM_RF(r, f);
    }

    return mask;
}



/* PIECE ATTACKS
 * ================== */

static bitboard knight_attacks(bitboard p)
{
    const bitboard l1 = FILE_SHIFT_RIGHT_1(p & ~FILEMASK_H);
    const bitboard l2 = FILE_SHIFT_RIGHT_2(p & ~(FILEMASK_G | FILEMASK_H));
    const bitboard r1 = FILE_SHIFT_LEFT_1 (p & ~FILEMASK_A);
    const bitboard r2 = FILE_SHIFT_LEFT_2 (p & ~(FILEMASK_A | FILEMASK_B));
    const bitboard h1 = l1 | r1;
    const bitboard h2 = l2 | r2;
    return RANK_SHIFT_UP_2(h1)
         | RANK_SHIFT_DOWN_2(h1)
         | RANK_SHIFT_UP_1(h2)
         | RANK_SHIFT_DOWN_1(h2);
}

/* rook_attacks_from_index computes rook attack sets from an index, and its
 * purpose is code generation (the magic bitboard).
 *
 * It does not discriminate between friendly and opponent pieces, the caller
 * should therefore do something like:
 * ```
 *   rook_attacks_from_index(p, friendly_occ | opponent_occ) & ~friendly_occ
 * ```
 * */
static bitboard rook_attacks_from_index_slow(enum square_index sq, bitboard occ)
{
    const enum rank_index rank = index_to_rank(sq);
    const enum file_index file = index_to_file(sq);

    bitboard atk = 0ULL;

    /* following loops assume rank and file types are unsigned, which relies on C23 enums */
    _Static_assert(((enum rank_index)0) - ((enum rank_index)1) > ((enum rank_index)0));
    _Static_assert(((enum file_index)0) - ((enum file_index)1) > ((enum file_index)0));

    for (enum rank_index walk_rank = rank+1; walk_rank <= RANK_INDEX_8; ++walk_rank) {
        const bitboard sq = SQUARE_MASK_FROM_RF(walk_rank, file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (enum rank_index walk_rank = rank-1; walk_rank <= RANK_INDEX_8; --walk_rank) {
        const bitboard sq = SQUARE_MASK_FROM_RF(walk_rank, file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (enum file_index walk_file = file+1; walk_file <= FILE_INDEX_H; ++walk_file) {
        const bitboard sq = SQUARE_MASK_FROM_RF(rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (enum file_index walk_file = file-1; walk_file <= FILE_INDEX_H; --walk_file) {
        const bitboard sq = SQUARE_MASK_FROM_RF(rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    
    return atk;
}

static bitboard rook_attacks_from_index(enum square_index sq, bitboard occ)
{
#ifdef CODEGEN
    return 0ULL;
#else
    #if ! __has_include("mbb_rook.h")
        #error "compile with -DCODEGEN and run once to generate required header files"
    #else

/* defines
   - `mbb_rook[SQUARE_INDEX_COUNT]`
   - `bitboard rook_attacks[SQUARE_INDEX_COUNT][1<<12ULL] */
#include "mbb_rook.h"

    const struct magic m = mbb_rook[sq];
    occ &= m.mask;
    occ *= m.magic;
    occ >>= (64ULL-12ULL);
    return rook_attacks[sq][occ];

    #endif
#endif
}

static bitboard rook_attacks(bitboard p, bitboard occ)
{
    bitboard b = 0ULL;
    while (p) {
        const index lsb = bitboard_lsb(p) - 1;
        p &= ~(1ULL<<(lsb));
        b |= rook_attacks_from_index(lsb, occ);
    }
    return b;
}

/* bishop_attacks_from_index computes bishop attack sets from an index, and its
 * purpose is code generation (the magic bitboard).
 *
 * It does not discriminate between friendly and opponent pieces, the caller
 * should therefore do something like:
 * ```
 *   bishop_attacks_from_index(p, occ) & ~friendly_occ
 * ```
 * */
static bitboard bishop_attacks_from_index_slow(enum square_index sq, bitboard occ)
{
    const enum rank_index rank = index_to_rank(sq);
    const enum file_index file = index_to_file(sq);

    bitboard atk = 0ULL;

    /* following loops assume rank and file types are unsigned, which relies on C23 enums */
    _Static_assert(((enum rank_index)0) - ((enum rank_index)1) > ((enum rank_index)0));
    _Static_assert(((enum file_index)0) - ((enum file_index)1) > ((enum file_index)0));

    enum rank_index walk_rank;
    enum file_index walk_file;
    for (walk_rank = rank+1, walk_file = file+1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            ++walk_rank, ++walk_file) {
        const bitboard sq = SQUARE_MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (walk_rank = rank+1, walk_file = file-1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            ++walk_rank, --walk_file) {
        const bitboard sq = SQUARE_MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (walk_rank = rank-1, walk_file = file+1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            --walk_rank, ++walk_file) {
        const bitboard sq = SQUARE_MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (walk_rank = rank-1, walk_file = file-1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            --walk_rank, --walk_file) {
        const bitboard sq = SQUARE_MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    
    return atk;
}

static bitboard bishop_attacks_from_index(enum square_index sq, bitboard occ)
{
#ifdef CODEGEN
    return 0ULL;
#else
    #if ! __has_include("mbb_bishop.h")
        #error "compile with -DCODEGEN and run once to generate required header files"
    #else

/* defines
   - `mbb_bishop[SQUARE_INDEX_COUNT]`
   - `bitboard bishop_attacks[SQUARE_INDEX_COUNT][1<<9ULL] */
#include "mbb_bishop.h"
    const struct magic m = mbb_bishop[sq];
    occ &= m.mask;
    occ *= m.magic;
    occ >>= (64ULL-9ULL);
    return bishop_attacks[sq][occ];
    #endif
#endif
}

static bitboard bishop_attacks(bitboard p, bitboard occ)
{
    bitboard b = 0ULL;
    while (p) {
        const index lsb = bitboard_lsb(p) - 1;
        p &= ~(1ULL<<(lsb));
        b |= bishop_attacks_from_index(lsb, occ);
    }
    return b;
}

static inline bitboard pawn_white_moves(bitboard p, bitboard empty)
{
    /* Assumptions:
       - empty = ~(black_occupied | white_occupied);
     */

    const bitboard single_push = RANK_SHIFT_UP(p, 1) & empty;

    const bitboard double_push =
        RANK_SHIFT_UP(single_push & RANKMASK_3, 1) & empty;

    return single_push | double_push;
}

static inline bitboard pawn_white_attacks(bitboard p, bitboard ep_targets, bitboard black_occupied)
{
    const bitboard up = RANK_SHIFT_UP(p, 1);
    const bitboard captures_right = FILE_SHIFT_RIGHT(up, 1) & ~FILEMASK_H;
    const bitboard captures_left  = FILE_SHIFT_LEFT(up, 1) & ~FILEMASK_A;

    return (captures_right | captures_left) & (black_occupied | ep_targets);
}

static inline bitboard pawn_black_moves(bitboard p, bitboard empty)
{
    /* Assumptions:
       - empty = ~(black_occupied | white_occupied);
     */

    const bitboard single_push = RANK_SHIFT_DOWN(p, 1) & empty;

    const bitboard double_push =
        RANK_SHIFT_DOWN(single_push & RANKMASK_6, 1) & empty;

    return single_push | double_push;
}

static inline bitboard pawn_black_attacks(bitboard p, bitboard ep_targets, bitboard white_occupied)
{
    const bitboard down = RANK_SHIFT_UP(p, 1);
    const bitboard captures_right = FILE_SHIFT_RIGHT(down, 1) & ~FILEMASK_H;
    const bitboard captures_left  = FILE_SHIFT_LEFT(down, 1) & ~FILEMASK_A;

    return (captures_right | captures_left) & (white_occupied | ep_targets);
}

static bitboard king_attacks(enum square_index sq)
{
    /* candidate for improvement */
    const bitboard n = RANK_SHIFT_UP(sq, 1);
    const bitboard s = RANK_SHIFT_DOWN(sq, 1);
    const bitboard w = FILE_SHIFT_LEFT(sq, 1);
    const bitboard e = FILE_SHIFT_RIGHT(sq, 1);

    const bitboard nw = FILE_SHIFT_LEFT(n, 1);
    const bitboard ne = FILE_SHIFT_RIGHT(n, 1);
    const bitboard sw = FILE_SHIFT_RIGHT(s, 1);
    const bitboard se = FILE_SHIFT_LEFT(s, 1);

    return n | s | w | e | nw | ne | sw | se;
}


/* BOARD
 * ================= */
enum player : index {
    PLAYER_BEGIN,
    PLAYER_WHITE = PLAYER_BEGIN,
    PLAYER_BLACK,
    PLAYER_COUNT,
};

enum piece : index {
    PIECE_BEGIN,
    PIECE_PAWN = PIECE_BEGIN,
    PIECE_KING,
    PIECE_QUEEN,
    PIECE_BISHOP,
    PIECE_ROOK,
    PIECE_KNIGHT,
    PIECE_COUNT,
};

const char piece_char[PLAYER_COUNT][PIECE_COUNT] = {
    [PLAYER_WHITE] = {
        [PIECE_PAWN]   = 'P',
        [PIECE_KING]   = 'K',
        [PIECE_QUEEN]  = 'Q',
        [PIECE_BISHOP] = 'B',
        [PIECE_ROOK]   = 'R',
        [PIECE_KNIGHT] = 'N',
    },
    [PLAYER_BLACK] = {
        [PIECE_PAWN]   = 'p',
        [PIECE_KING]   = 'k',
        [PIECE_QUEEN]  = 'q',
        [PIECE_BISHOP] = 'b',
        [PIECE_ROOK]   = 'r',
        [PIECE_KNIGHT] = 'n',
    }
};

const int piece_unicode[PLAYER_COUNT][PIECE_COUNT] = {
    [PLAYER_WHITE] = {
        [PIECE_PAWN]   = 0x2659,
        [PIECE_KING]   = 0x2654,
        [PIECE_QUEEN]  = 0x2655,
        [PIECE_BISHOP] = 0x2657,
        [PIECE_ROOK]   = 0x2656,
        [PIECE_KNIGHT] = 0x2658,
    },
    [PLAYER_BLACK] = {
        [PIECE_PAWN]   = 0x265F,
        [PIECE_KING]   = 0x265A,
        [PIECE_QUEEN]  = 0x265B,
        [PIECE_BISHOP] = 0x265D,
        [PIECE_ROOK]   = 0x265C,
        [PIECE_KNIGHT] = 0x265E,
    }
};

struct board {
    bitboard pieces[PLAYER_COUNT][PIECE_COUNT];
    bitboard enpassent_targets[PLAYER_COUNT];
    bitboard occupied[PLAYER_COUNT];
};

#define BOARD_INITIAL (struct board) {                        \
    .pieces = {                                               \
        [PLAYER_WHITE] = {                                    \
            [PIECE_PAWN]   = RANKMASK_2,                      \
            [PIECE_ROOK]   = SQUARE_MASK_A1 | SQUARE_MASK_H1, \
            [PIECE_KNIGHT] = SQUARE_MASK_B1 | SQUARE_MASK_G1, \
            [PIECE_BISHOP] = SQUARE_MASK_C1 | SQUARE_MASK_F1, \
            [PIECE_QUEEN]  = SQUARE_MASK_D1,                  \
            [PIECE_KING]   = SQUARE_MASK_E1,                  \
        },                                                    \
        [PLAYER_BLACK] = {                                    \
            [PIECE_PAWN]   = RANKMASK_7,                      \
            [PIECE_ROOK]   = SQUARE_MASK_A8 | SQUARE_MASK_H8, \
            [PIECE_KNIGHT] = SQUARE_MASK_B8 | SQUARE_MASK_G8, \
            [PIECE_BISHOP] = SQUARE_MASK_C8 | SQUARE_MASK_F8, \
            [PIECE_QUEEN]  = SQUARE_MASK_D8,                  \
            [PIECE_KING]   = SQUARE_MASK_E8,                  \
        }                                                     \
    }                                                         \
}

void board_print(struct board* b, FILE* out)
{
    int buf[8][8] = {0};

    for (size_t i = 0; i < 8; i++) {
        for (size_t i = 0; i < 8; i++) {
            buf[i][i] = 0;
        }
    }

    for (enum player player = PLAYER_BEGIN; player < PLAYER_COUNT; player++) {
        for (enum piece piece = PIECE_BEGIN; piece < PIECE_COUNT; piece++) {
            bitboard x = b->pieces[player][piece];

            for (index i = 7; i < 8; i--) {
                for (index j = 0; j < 8; j++) {
                    if (x & (1ULL<<(i*8+j))) {
                        buf[i][j] = piece_unicode[player][piece];
                    }
                }
            }
        }
    }

    /* see: https://en.wikipedia.org/wiki/ANSI_escape_code#Colors */
    setlocale(LC_ALL, "");
    for (index i = 7; i < 8; i--) {
        fprintf(out, "\033[0m"); /* reset background color */
        fprintf(out, "%"INDEX_FMT" ", i+1);
        for (index j = 0; j < 8; j++) {
            /* 45: magenta, 47: white */
            fprintf(out, "\033[%d;%dm", 30, (i+j) % 2 ? 45 : 47);
            if (buf[i][j]) {
                fprintf(out, "%lc ", buf[i][j]);
            } else {
                fprintf(out, "  "); /* idk why this hack is needed but "%lc "
                                       is not working when buf[i][j] = ' ' */
            }
        }
        fprintf(out, "\033[0m"); /* reset background color */
        fprintf(out, "\n");
    }
    fprintf(out, "  A B C D E F G H \n");
}

/* debug functions
 * =============== */

void bitboard_print(bitboard b, FILE* out)
{
    setlocale(LC_ALL, "");
    fprintf(out, "\n");
    for (index i = 7; i < 8; i--) {
        fprintf(out, "\033[0m"); /* reset background color */
        fprintf(out, "%"INDEX_FMT" ", i+1);
        for (index j = 0; j < 8; j++) {
            const index n = i*8+j;
            int color;
            if ((b >> n) & 1) {
                /* 41: red */
                color = 41;
            } else {
                /* 45: magenta, 47: white */
                color = (i+j)%2 ? 45 : 47;
            }
            fprintf(out, "\033[30;%dm", color);
            fprintf(out, "  ");
        }
        fprintf(out, "\033[0m"); /* reset background color */
        fprintf(out, "\n");
    }
    fprintf(out, "  A B C D E F G H \n");
}

