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
#define SQ_MASK_FROM_RF(rank, file) ((index)1ULL << INDEX_FROM_RF(rank,file))

#define SQ_MASK_FROM_INDEX(idx) (1ULL<<(idx))

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

#define STR(x) #x

#define SQ_MASK_PREFIX SQ_MASK_
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
    X(H, 8)

/* define mask constants: SQ_MASK_A1, SQ_MASK_A2, ..., SQ_MASK_H8 */
enum : bitboard {
    #define X(file, rank) SQ_MASK_##file##rank = SQ_MASK_FROM_RF(RANK_INDEX_##rank, FILE_INDEX_##file),
    SQUARES_LIST
    #undef X
};

enum square_index : bitboard {
    #define X(file, rank) SQ_INDEX_##file##rank = INDEX_FROM_RF(RANK_INDEX_##rank, FILE_INDEX_##file),
    SQUARES_LIST
    SQ_INDEX_COUNT,
    #undef X
    /* define iterator begin enum */
    #define X(file, rank) SQ_INDEX_BEGIN = SQ_INDEX_##file##rank,
    SQUARES_LIST_BEGIN
    #undef X
};

const char* square_index_display[SQ_INDEX_COUNT] = {
    #define X(file, rank) \
    [SQ_INDEX_##file##rank] = STR(file##rank),
    SQUARES_LIST
    #undef X
};

const char* square_index_str[SQ_INDEX_COUNT] = {
    #define X(file, rank) \
    [SQ_INDEX_##file##rank] = STR(SQ_INDEX_##file##rank),
    SQUARES_LIST
    #undef X
};

enum : bitboard {
    FILE_MASK_A = FILE_MASK(FILE_INDEX_A),
    FILE_MASK_B = FILE_MASK(FILE_INDEX_B),
    FILE_MASK_C = FILE_MASK(FILE_INDEX_C),
    FILE_MASK_D = FILE_MASK(FILE_INDEX_D),
    FILE_MASK_E = FILE_MASK(FILE_INDEX_E),
    FILE_MASK_F = FILE_MASK(FILE_INDEX_F),
    FILE_MASK_G = FILE_MASK(FILE_INDEX_G),
    FILE_MASK_H = FILE_MASK(FILE_INDEX_H),

    RANK_MASK_1 = RANK_MASK(RANK_INDEX_1),
    RANK_MASK_2 = RANK_MASK(RANK_INDEX_2),
    RANK_MASK_3 = RANK_MASK(RANK_INDEX_3),
    RANK_MASK_4 = RANK_MASK(RANK_INDEX_4),
    RANK_MASK_5 = RANK_MASK(RANK_INDEX_5),
    RANK_MASK_6 = RANK_MASK(RANK_INDEX_6),
    RANK_MASK_7 = RANK_MASK(RANK_INDEX_7),
    RANK_MASK_8 = RANK_MASK(RANK_INDEX_8),

    FILE_SHIFT_RIGHT_GUARD_0 = ~0ULL,
    FILE_SHIFT_RIGHT_GUARD_1 = ~FILE_MASK_H,
    FILE_SHIFT_RIGHT_GUARD_2 = ~(FILE_MASK_G | FILE_MASK_H),
    FILE_SHIFT_RIGHT_GUARD_3 = ~(FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),
    FILE_SHIFT_RIGHT_GUARD_4 = ~(FILE_MASK_E | FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),
    FILE_SHIFT_RIGHT_GUARD_5 = ~(FILE_MASK_D | FILE_MASK_E | FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),
    FILE_SHIFT_RIGHT_GUARD_6 = ~(FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),
    FILE_SHIFT_RIGHT_GUARD_7 = ~(FILE_MASK_B | FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),

    FILE_SHIFT_LEFT_GUARD_0  = ~0ULL,
    FILE_SHIFT_LEFT_GUARD_1  = ~FILE_MASK_A,
    FILE_SHIFT_LEFT_GUARD_2  = ~(FILE_MASK_A | FILE_MASK_B),
    FILE_SHIFT_LEFT_GUARD_3  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C),
    FILE_SHIFT_LEFT_GUARD_4  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C | FILE_MASK_D),
    FILE_SHIFT_LEFT_GUARD_5  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C | FILE_MASK_D | FILE_MASK_E),
    FILE_SHIFT_LEFT_GUARD_6  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F),
    FILE_SHIFT_LEFT_GUARD_7  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F | FILE_MASK_G),
};



#define _FILE_SHIFT_RIGHT_GUARDED(b, n) \
    (((bitboard)(b) & FILE_SHIFT_RIGHT_GUARD_##n) << (index)(n))

#define _FILE_SHIFT_LEFT_GUARDED(b, n)  \
    (((bitboard)(b) & FILE_SHIFT_LEFT_GUARD_##n) >> (index)(n))

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
    return __builtin_ffsll(p) - 1;
}

static index bitboard_pop_lsb(bitboard* p)
{
    const index lsb = bitboard_lsb(*p);
    *p &= ~(1ULL<<(lsb));
    return lsb;
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
        & ~SQ_MASK_FROM_INDEX(p);
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
        mask |= SQ_MASK_FROM_RF(r, f);
    }

    /* NW (rank+1, file-1) */
    for (r = rank+1, f = file-1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         ++r, --f)
    {
        mask |= SQ_MASK_FROM_RF(r, f);
    }

    /* SE (rank-1, file+1) */
    for (r = rank-1, f = file+1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         --r, ++f)
    {
        mask |= SQ_MASK_FROM_RF(r, f);
    }

    /* SW (rank-1, file-1) */
    for (r = rank-1, f = file-1;
         r <= RANK_INDEX_8 && f <= FILE_INDEX_H;
         --r, --f)
    {
        mask |= SQ_MASK_FROM_RF(r, f);
    }

    return mask;
}



/* PIECE ATTACKS
 * ==================
 *
 * All piece attack functions rely on the caller masking their own pieces.
 * e.g. `knight_attacks(knights) & ~own_occupied`
 * */

static bitboard knight_attacks(bitboard p)
{
    const bitboard l1 = FILE_SHIFT_RIGHT_1(p & ~FILE_MASK_H);
    const bitboard l2 = FILE_SHIFT_RIGHT_2(p & ~(FILE_MASK_G | FILE_MASK_H));
    const bitboard r1 = FILE_SHIFT_LEFT_1 (p & ~FILE_MASK_A);
    const bitboard r2 = FILE_SHIFT_LEFT_2 (p & ~(FILE_MASK_A | FILE_MASK_B));
    const bitboard h1 = l1 | r1;
    const bitboard h2 = l2 | r2;
    return RANK_SHIFT_UP_2(h1)
         | RANK_SHIFT_DOWN_2(h1)
         | RANK_SHIFT_UP_1(h2)
         | RANK_SHIFT_DOWN_1(h2);
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
   - `mbb_rook[SQ_INDEX_COUNT]`
   - `bitboard rook_attacks[SQ_INDEX_COUNT][1<<12ULL] */
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
        const index lsb = bitboard_pop_lsb(&p);
        b |= rook_attacks_from_index(lsb, occ);
    }
    return b;
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
   - `mbb_bishop[SQ_INDEX_COUNT]`
   - `bitboard bishop_attacks[SQ_INDEX_COUNT][1<<9ULL] */
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
        const index lsb = bitboard_pop_lsb(&p);
        b |= bishop_attacks_from_index(lsb, occ);
    }
    return b;
}

static bitboard queen_attacks_from_index(enum square_index sq, bitboard occ)
{
    return bishop_attacks_from_index(sq, occ) | rook_attacks_from_index(sq, occ);
}

static bitboard queen_attacks(bitboard p, bitboard occ)
{
    bitboard b = 0ULL;
    while (p) {
        const index lsb = bitboard_pop_lsb(&p);
        b |= queen_attacks_from_index(lsb, occ);
    }
    return b;
}

static inline bitboard pawn_moves_white(bitboard p, bitboard empty)
{
    /* Assumptions:
       - empty = ~(black_occupied | white_occupied);
     */

    const bitboard single_push = RANK_SHIFT_UP_1(p) & empty;

    const bitboard double_push =
        RANK_SHIFT_UP_1(single_push & RANK_MASK_3) & empty;

    return (single_push | double_push);
}

static inline bitboard pawn_attacks_white(bitboard p)
{
    const bitboard up = RANK_SHIFT_UP_1(p);
    const bitboard captures_right = FILE_SHIFT_RIGHT_1(up);
    const bitboard captures_left  = FILE_SHIFT_LEFT_1(up);

    return (captures_right | captures_left);
}

static inline bitboard pawn_moves_black(bitboard p, bitboard empty)
{
    /* Assumptions:
       - empty = ~(black_occupied | white_occupied);
     */

    const bitboard single_push = RANK_SHIFT_DOWN_1(p) & empty;

    const bitboard double_push =
        RANK_SHIFT_DOWN_1(single_push & RANK_MASK_6) & empty;

    return (single_push | double_push);
}

static inline bitboard pawn_attacks_black(bitboard p)
{
    const bitboard down = RANK_SHIFT_DOWN_1(p);
    const bitboard captures_right = FILE_SHIFT_RIGHT_1(down);
    const bitboard captures_left  = FILE_SHIFT_LEFT_1(down);

    return (captures_right | captures_left);
}

static bitboard king_attacks(enum square_index sq)
{
    /* candidate for improvement */
    const bitboard n = RANK_SHIFT_UP_1(sq);
    const bitboard s = RANK_SHIFT_DOWN_1(sq);
    const bitboard w = FILE_SHIFT_LEFT_1(sq);
    const bitboard e = FILE_SHIFT_RIGHT_1(sq);

    const bitboard nw = FILE_SHIFT_LEFT_1(n);
    const bitboard ne = FILE_SHIFT_RIGHT_1(n);
    const bitboard sw = FILE_SHIFT_RIGHT_1(s);
    const bitboard se = FILE_SHIFT_LEFT_1(s);

    return (n | s | w | e | nw | ne | sw | se);
}

/* BOARD
 * ================= */
enum player : index {
    PLAYER_BEGIN,
    PLAYER_WHITE = PLAYER_BEGIN,
    PLAYER_BLACK,
    PLAYER_COUNT,
};

static inline enum player opposite_player(enum player p)
{
    return (p == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE);
} 

const char* player_str[PLAYER_COUNT] = {
    [PLAYER_WHITE] = "PLAYER_WHITE",
    [PLAYER_BLACK] = "PLAYER_BLACK",
};

#define PIECE_BEGIN PIECE_PAWN
/*    enum          value       white char   white unicode   black char    black unicode  */
#define PIECES \
    X(PIECE_PAWN,     1.0,             'P',         0x2659,         'p',          0x265F) \
    X(PIECE_KING,     5.0,             'K',         0x2654,         'k',          0x265A) \
    X(PIECE_QUEEN,    9.0,             'Q',         0x2655,         'q',          0x265B) \
    X(PIECE_BISHOP,   3.0,             'Q',         0x2657,         'q',          0x265D) \
    X(PIECE_ROOK,     5.0,             'R',         0x2656,         'q',          0x265C) \
    X(PIECE_KNIGHT,   3.0,             'N',         0x2658,         'n',          0x265E)

enum piece : size_t {
#define X(e, v, wc, wu, bc, bu) e,
    PIECES
    PIECE_COUNT,
#undef X
};

double piece_value[PIECE_COUNT] = {
#define X(e, v, wc, wu, bc, bu) [e] = v,
    PIECES
#undef X
};

const char* piece_str[PIECE_COUNT] = {
#define X(e, v, wc, wu, bc, bu) [e] = STR(e),
    PIECES
#undef X
};

const char piece_char[PLAYER_COUNT][PIECE_COUNT] = {
    [PLAYER_WHITE] = {
#define X(e, v, wc, wu, bc, bu) [e] = wc,
        PIECES
#undef X
    },
    [PLAYER_BLACK] = {
#define X(e, v, wc, wu, bc, bu) [e] = bc,
        PIECES
#undef X
    }
};

const int piece_unicode[PLAYER_COUNT][PIECE_COUNT] = {
    [PLAYER_WHITE] = {
#define X(e, v, wc, wu, bc, bu) [e] = wu,
        PIECES
#undef X
    },
    [PLAYER_BLACK] = {
#define X(e, v, wc, wu, bc, bu) [e] = bu,
        PIECES
#undef X
    }
};

struct board {
    bitboard pieces[PLAYER_COUNT][PIECE_COUNT];
    bitboard ep_targets;
    bitboard occupied[PLAYER_COUNT];

    /* Used only for square->piece lookups, not central to logic.
     * Does not store which player owns the square piece */
    enum piece mailbox[SQ_INDEX_COUNT];
};

#define BOARD_INITIAL (struct board) {                \
    .pieces = {                                       \
        [PLAYER_WHITE] = {                            \
            [PIECE_PAWN]   = RANK_MASK_2,             \
            [PIECE_ROOK]   = SQ_MASK_A1 | SQ_MASK_H1, \
            [PIECE_KNIGHT] = SQ_MASK_B1 | SQ_MASK_G1, \
            [PIECE_BISHOP] = SQ_MASK_C1 | SQ_MASK_F1, \
            [PIECE_QUEEN]  = SQ_MASK_D1,              \
            [PIECE_KING]   = SQ_MASK_E1,              \
        },                                            \
        [PLAYER_BLACK] = {                            \
            [PIECE_PAWN]   = RANK_MASK_7,             \
            [PIECE_ROOK]   = SQ_MASK_A8 | SQ_MASK_H8, \
            [PIECE_KNIGHT] = SQ_MASK_B8 | SQ_MASK_G8, \
            [PIECE_BISHOP] = SQ_MASK_C8 | SQ_MASK_F8, \
            [PIECE_QUEEN]  = SQ_MASK_D8,              \
            [PIECE_KING]   = SQ_MASK_E8,              \
        }                                             \
    },                                                \
    .ep_targets = 0ULL,                               \
    .occupied = {                                     \
        [PLAYER_WHITE] =                              \
            RANK_MASK_2 | SQ_MASK_A1 | SQ_MASK_H1 |   \
            SQ_MASK_B1 | SQ_MASK_G1 | SQ_MASK_C1 |    \
            SQ_MASK_F1 | SQ_MASK_D1 | SQ_MASK_E1,     \
        [PLAYER_BLACK] =                              \
            RANK_MASK_7 | SQ_MASK_A8 | SQ_MASK_H8 |   \
            SQ_MASK_B8 | SQ_MASK_G8| SQ_MASK_C8 |     \
            SQ_MASK_F8| SQ_MASK_D8| SQ_MASK_E8,       \
    },                                                \
    .mailbox = {                                      \
        [SQ_INDEX_A1] = PIECE_ROOK,                   \
        [SQ_INDEX_B1] = PIECE_KNIGHT,                 \
        [SQ_INDEX_C1] = PIECE_BISHOP,                 \
        [SQ_INDEX_D1] = PIECE_QUEEN,                  \
        [SQ_INDEX_E1] = PIECE_KING,                   \
        [SQ_INDEX_F1] = PIECE_BISHOP,                 \
        [SQ_INDEX_G1] = PIECE_KNIGHT,                 \
        [SQ_INDEX_H1] = PIECE_ROOK,                   \
        [SQ_INDEX_A2] = PIECE_PAWN,                   \
        [SQ_INDEX_B2] = PIECE_PAWN,                   \
        [SQ_INDEX_C2] = PIECE_PAWN,                   \
        [SQ_INDEX_D2] = PIECE_PAWN,                   \
        [SQ_INDEX_E2] = PIECE_PAWN,                   \
        [SQ_INDEX_F2] = PIECE_PAWN,                   \
        [SQ_INDEX_G2] = PIECE_PAWN,                   \
        [SQ_INDEX_H2] = PIECE_PAWN,                   \
        [SQ_INDEX_A7] = PIECE_PAWN,                   \
        [SQ_INDEX_B7] = PIECE_PAWN,                   \
        [SQ_INDEX_C7] = PIECE_PAWN,                   \
        [SQ_INDEX_D7] = PIECE_PAWN,                   \
        [SQ_INDEX_E7] = PIECE_PAWN,                   \
        [SQ_INDEX_F7] = PIECE_PAWN,                   \
        [SQ_INDEX_G7] = PIECE_PAWN,                   \
        [SQ_INDEX_H7] = PIECE_PAWN,                   \
        [SQ_INDEX_A8] = PIECE_ROOK,                   \
        [SQ_INDEX_B8] = PIECE_KNIGHT,                 \
        [SQ_INDEX_C8] = PIECE_BISHOP,                 \
        [SQ_INDEX_D8] = PIECE_QUEEN,                  \
        [SQ_INDEX_E8] = PIECE_KING,                   \
        [SQ_INDEX_F8] = PIECE_BISHOP,                 \
        [SQ_INDEX_G8] = PIECE_KNIGHT,                 \
        [SQ_INDEX_H8] = PIECE_ROOK,                   \
    },                                                \
}

double board_score_heuristic(const struct board* board, enum player player)
{
    double score = 0.0;

    enum player opp = opposite_player(player);

    for (enum piece piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
        score += (double)bitboard_popcount(board->pieces[player][piece]) * piece_value[piece];
    }
    for (enum piece piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
        score -= (double)bitboard_popcount(board->pieces[opp][piece]) * piece_value[piece];
    }

    return score;
}

struct move {
    index from;
    index to;
};

struct move move(index from, index to)
{
    return (struct move){
        .from = from,
        .to = to
    };
}

#define MOVE_MAX 32

static size_t all_moves_from(const struct board* board,
                            enum player          player,
                            enum square_index    from,
                            struct move          moves[static MOVE_MAX])
{
    const enum piece piece = board->mailbox[from];

    const enum player opp = opposite_player(player);

    const bitboard own_occ = board->occupied[player];
    const bitboard opp_occ = board->occupied[opp];
    const bitboard occ = own_occ | opp_occ;

    const bitboard sqmask = SQ_MASK_FROM_INDEX(from);

    bitboard x = 0ULL;

    switch (piece) {
        case PIECE_PAWN: {
            if (player == PLAYER_WHITE) {
                x |= pawn_moves_white(sqmask, ~occ);
                x |= pawn_attacks_white(sqmask) & (board->ep_targets | opp_occ);
            } else {
                x |= pawn_moves_black(sqmask, ~occ);
                x |= pawn_attacks_black(sqmask) & (board->ep_targets | opp_occ);
            }
        } break;

        case PIECE_KNIGHT: {
            x |= knight_attacks(sqmask);
        } break;

        case PIECE_BISHOP: {
            x |= bishop_attacks_from_index(from, occ);
        } break;

        case PIECE_ROOK: {
            x |= rook_attacks_from_index(from, occ);
        } break;

        case PIECE_QUEEN: {
            x |= queen_attacks_from_index(from, occ);
        } break;

        case PIECE_KING: {
            x |= king_attacks(sqmask);
        } break;
    }

    x &= ~board->occupied[player];

    size_t move_count = 0;
    while (x) {
        const index to = bitboard_pop_lsb(&x);
        moves[move_count++] = (struct move){.from = from, .to = to};
    }
    return move_count;
}

static size_t all_player_pieces(struct board* board,
                                enum player player,
                                enum square_index out[static SQ_INDEX_COUNT])
{
    size_t count = 0;

    fprintf(stderr, "%s\n", player_str[player]);
    for (enum piece piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
        bitboard x = board->pieces[player][piece];
        fprintf(stderr, "%s:\t%016"BITBOARD_FMT_X"\n", piece_str[piece], x);
        while (x) {
            out[count++] = (enum square_index)bitboard_pop_lsb(&x);
        }
    }

    return count;
}

/* caller is resposible for checking validity */
static void board_move_piece(struct board* board,
                             enum player player,
                             struct move move)
{
    const bitboard from_mask = SQ_MASK_FROM_INDEX(move.from);
    const bitboard to_mask = SQ_MASK_FROM_INDEX(move.to);

    const enum player opp = opposite_player(player);
    const enum piece from_piece = board->mailbox[move.from];
    const enum piece to_piece   = board->mailbox[move.to];

    board->pieces[player][from_piece] &= ~from_mask;
    board->pieces[player][from_piece] |= to_mask;
    board->pieces[opp][to_piece] &= ~to_mask;

    board->occupied[player] &= ~from_mask;
    board->occupied[player] |= to_mask;
    board->occupied[opp] &= ~to_mask;

    /* mailboxes are solely used for fast lookups of pieces in already valid
     * moves, it's safe to ignore the from index */
    board->mailbox[move.to] = from_piece;
}

static bool board_is_check(struct board* b, enum player player)
{
    const bitboard kb = b->pieces[player][PIECE_KING];

    const enum player opp = opposite_player(player);

    const bitboard player_occ = bitboard->occupied[player];
    const bitboard opp_occ = bitboard->occupied[opp];
    const bitboard occ = player_occ | opp_occ;
    const bitboard* p = b->pieces[opp];

    bitboard threats = 0ULL;
    if (player == PLAYER_WHITE) {
        threats |= pawn_attacks_white(p[PIECE_PAWN]);
    } else {
        threats |= pawn_attacks_black(p[PIECE_PAWN]);
    }
    threats |= bishop_attacks(p[PIECE_BISHOP], occ);
    threats |= rook_attacks(p[PIECE_ROOK], occ);
    threats |= king_attacks(p[PIECE_KING]);
    threats |= queen_attacks(p[PIECE_QUEEN], occ);
    threats |= knight_attacks(p[PIECE_KNIGHT]);
    threats &= ~player_occ;

    if (threats & kb) {
        return true;
    }
    return false;
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

void board_print_threats(struct board* b, FILE* out, enum player player)
{
    int buf[8][8] = {0};

    for (size_t i = 0; i < 8; i++) {
        for (size_t i = 0; i < 8; i++) {
            buf[i][i] = 0;
        }
    }

    enum player opp = opposite_player(player);
    bitboard opp_occ = 0ULL,
             own_occ = 0ULL;
    for (enum piece piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
        opp_occ |= b->pieces[opp][piece];
        own_occ |= b->pieces[player][piece];
    }
    const bitboard occ = opp_occ | own_occ;

    bitboard threats = 0ULL;

    const bitboard* p = b->pieces[player];
    if (player == PLAYER_WHITE) {
        threats |= pawn_attacks_white(p[PIECE_PAWN]);
    } else {
        threats |= pawn_attacks_black(p[PIECE_PAWN]);
    }
    threats |= bishop_attacks(p[PIECE_BISHOP], occ);
    threats |= rook_attacks(p[PIECE_ROOK], occ);
    threats |= king_attacks(p[PIECE_KING]);
    threats |= queen_attacks(p[PIECE_QUEEN], occ);
    threats |= knight_attacks(p[PIECE_KNIGHT]);
    threats &= ~own_occ;

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
            const index n = i*8+j;
            if ((threats >> n) & 1) {
                fprintf(out, "\033[%d;%dm", 30, 41);
            } else {
                /* 45: magenta, 47: white */
                fprintf(out, "\033[%d;%dm", 30, (i+j) % 2 ? 45 : 47);
            }
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
