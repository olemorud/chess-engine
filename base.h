#pragma once

#include <ctype.h>
#include <math.h>
#include <locale.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <string.h>


static struct {
    /* all globals are zero by default in C */
    uint64_t tt_collisions;
    uint64_t tt_hits;
    uint64_t tt_probes;
} global_stats;

static void print_stats(FILE* out)
{
    fprintf(out, "Stats:\n");
    fprintf(out, "tt collisions: %lu\n", global_stats.tt_collisions);
    fprintf(out, "tt hits:       %lu\n", global_stats.tt_hits);
    fprintf(out, "tt probes:     %lu\n", global_stats.tt_probes);
}

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
    return (uint64_t)__builtin_ctzll(n);
}

/**/
static uint64_t rand64() {
    union {
        uint64_t v;
        uint16_t vs[4];
    } x = {
        .vs = {
            (uint16_t)rand(),
            (uint16_t)rand(),
            (uint16_t)rand(),
            (uint16_t)rand(),
        }
    };

    return x.v;
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

typedef uint8_t index;
#define INDEX(n) n##ULL
#define INDEX_FMT PRIu8

#define RANK_SHIFT_UP(b, n)    ((b) << ((index)(n)*8U))
#define RANK_SHIFT_DOWN(b, n)  ((b) >> ((index)(n)*8U))
#define FILE_SHIFT_LEFT(b, n)  ((b) >> ((index)(n)*1U))
#define FILE_SHIFT_RIGHT(b, n) ((b) << ((index)(n)*1U))

#define INDEX_SHIFT_UP(i, n)    ((i) + ((index)(n)*8U))
#define INDEX_SHIFT_DOWN(i, n)  ((i) - ((index)(n)*8U))
#define INDEX_SHIFT_LEFT(i, n)  ((i) - ((index)(n)*1U))
#define INDEX_SHIFT_RIGHT(i, n) ((i) + ((index)(n)*1U))

#define FILE_MASK(n) FILE_SHIFT_RIGHT((bitboard)0x0101010101010101ULL, n)
#define RANK_MASK(n) RANK_SHIFT_UP((bitboard)0x00000000000000FFULL, n)

#define INDEX_FROM_RF(rank, file) ((index)(8U*(index)(rank) + (index)(file)))

#define SQ_MASK_FROM_INDEX(idx) (((bitboard)1ULL)<<((index)idx))
#define SQ_MASK_FROM_RF(rank, file) (SQ_MASK_FROM_INDEX(INDEX_FROM_RF(rank,file)))

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

enum square_index : index {
    #define X(file, rank) SQ_INDEX_##file##rank = INDEX_FROM_RF(RANK_INDEX_##rank, FILE_INDEX_##file),
    SQUARES_LIST
    SQ_INDEX_COUNT,
    #undef X
    /* define iterator begin enum */
    #define X(file, rank) SQ_INDEX_BEGIN = SQ_INDEX_##file##rank,
    SQUARES_LIST_BEGIN
    #undef X
};

static char* const square_index_display[SQ_INDEX_COUNT] = {
    #define X(file, rank) \
    [SQ_INDEX_##file##rank] = STR(file##rank),
    SQUARES_LIST
    #undef X
};

static char* const square_index_str[SQ_INDEX_COUNT] = {
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

/*
 *  Player
 *  ===========
 * */
enum player : size_t {
    PLAYER_BEGIN,
    PLAYER_WHITE = PLAYER_BEGIN,
    PLAYER_BLACK,
    PLAYER_COUNT,
};

static inline enum player opposite_player(enum player p)
{
    return (p == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE);
} 

static char const* player_str[PLAYER_COUNT] = {
    [PLAYER_WHITE] = "PLAYER_WHITE",
    [PLAYER_BLACK] = "PLAYER_BLACK",
};

/*
 *  Piece
 *  ===========
 * */
/*    enum          value       white char   white unicode   black char    black unicode  */
#define PIECES \
    X(PIECE_PAWN,     1.0,             'P',         0x2659,         'p',          0x265F) \
    X(PIECE_KING,     5.0,             'K',         0x2654,         'k',          0x265A) \
    X(PIECE_QUEEN,    9.0,             'Q',         0x2655,         'q',          0x265B) \
    X(PIECE_BISHOP,   3.0,             'B',         0x2657,         'b',          0x265D) \
    X(PIECE_ROOK,     5.0,             'R',         0x2656,         'r',          0x265C) \
    X(PIECE_KNIGHT,   3.0,             'N',         0x2658,         'n',          0x265E)

enum piece : uint8_t {
#define X(e, v, wc, wu, bc, bu) e,
    PIECES
    PIECE_COUNT,
    PIECE_BEGIN = 0,
#undef X
};

static double piece_value[PIECE_COUNT] = {
#define X(e, v, wc, wu, bc, bu) [e] = v,
    PIECES
#undef X
};

static char const* piece_str[PIECE_COUNT] = {
#define X(e, v, wc, wu, bc, bu) [e] = STR(e),
    PIECES
#undef X
};

struct piece_player {enum piece piece; enum player player;} const piece_and_player_from_char[256] = {
    #define X(e, v, wc, wu, bc, bu) \
        [(uint8_t)bc] = {.piece = e, .player = PLAYER_BLACK}, \
        [(uint8_t)wc] = {.piece = e, .player = PLAYER_WHITE},
    PIECES
    #undef X
};

static char const piece_char[PLAYER_COUNT][PIECE_COUNT] = {
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

static int const piece_unicode[PLAYER_COUNT][PIECE_COUNT] = {
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

static enum square_index bitboard_lsb(bitboard p)
{
    return (enum square_index)__builtin_ffsll((int64_t)p) - 1;
}

static enum square_index bitboard_pop_lsb(bitboard* p)
{
    index const lsb = bitboard_lsb(*p);
    *p &= ~(1ULL<<(lsb));
    return lsb;
}

static uint64_t bitboard_popcount(bitboard b)
{
    return (uint64_t)__builtin_popcountll(b);
}

static inline enum file_index index_to_file(enum square_index p)
{
    return p % 8ULL;
}

static inline enum rank_index index_to_rank(enum square_index p)
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
    enum rank_index const rank = index_to_rank(sq);
    enum file_index const file = index_to_file(sq);

    bitboard mask = 0ULL;

    /* Ensure signed-underflow detection rules match your style */
    _Static_assert(((enum rank_index)-1) > 0, "rank_index must be unsigned");
    _Static_assert(((enum file_index)-1) > 0, "file_index must be unsigned");

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
 * e.g. `knight_attacks(knights) & ~our_occupied`
 * */

static bitboard knight_attacks(bitboard p)
{
    bitboard const l1 = FILE_SHIFT_RIGHT_1(p /*& ~FILE_MASK_H*/);
    bitboard const l2 = FILE_SHIFT_RIGHT_2(p /*& ~(FILE_MASK_G | FILE_MASK_H)*/);
    bitboard const r1 = FILE_SHIFT_LEFT_1 (p /*& ~FILE_MASK_A*/);
    bitboard const r2 = FILE_SHIFT_LEFT_2 (p /*& ~(FILE_MASK_A | FILE_MASK_B)*/);
    bitboard const h1 = l1 | r1;
    bitboard const h2 = l2 | r2;
    return RANK_SHIFT_UP_2(h1)
         | RANK_SHIFT_DOWN_2(h1)
         | RANK_SHIFT_UP_1(h2)
         | RANK_SHIFT_DOWN_1(h2);
}


static bitboard rook_attacks_from_index(enum square_index sq, bitboard occ)
{
#ifdef CODEGEN
    enum rank_index const rank = index_to_rank(sq);
    enum file_index const file = index_to_file(sq);

    bitboard atk = 0ULL;

    /* following loops assume rank and file types are unsigned, which relies on C23 enums */
    _Static_assert(((enum rank_index)-1) > 0, "rank_index must be unsigned");
    _Static_assert(((enum file_index)-1) > 0, "file_index must be unsigned");

    for (enum rank_index walk_rank = rank+1; walk_rank <= RANK_INDEX_8; ++walk_rank) {
        bitboard const sq = SQ_MASK_FROM_RF(walk_rank, file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (enum rank_index walk_rank = rank-1; walk_rank <= RANK_INDEX_8; --walk_rank) {
        bitboard const sq = SQ_MASK_FROM_RF(walk_rank, file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (enum file_index walk_file = file+1; walk_file <= FILE_INDEX_H; ++walk_file) {
        bitboard const sq = SQ_MASK_FROM_RF(rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (enum file_index walk_file = file-1; walk_file <= FILE_INDEX_H; --walk_file) {
        bitboard const sq = SQ_MASK_FROM_RF(rank, walk_file);
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
       - `mbb_rook[SQ_INDEX_COUNT]`
       - `bitboard rook_attacks[SQ_INDEX_COUNT][1<<12ULL] */
    #include "mbb_rook.h"

    struct magic const m = mbb_rook[sq];
    occ &= m.mask;
    occ *= m.magic;
    occ >>= (64ULL-12ULL);
    assert(rook_attacks[sq][occ] != SQ_MASK_FROM_INDEX(sq));
    return rook_attacks[sq][occ];

    #endif
#endif
}

static bitboard rook_attacks(bitboard p, bitboard occ)
{
    bitboard b = 0ULL;
    while (p) {
        index const lsb = bitboard_pop_lsb(&p);
        b |= rook_attacks_from_index(lsb, occ);
    }
    return b;
}

static bitboard bishop_attacks_from_index(enum square_index sq, bitboard occ)
{
#ifdef CODEGEN
    enum rank_index const rank = index_to_rank(sq);
    enum file_index const file = index_to_file(sq);

    bitboard atk = 0ULL;

    /* following loops assume rank and file types are unsigned, which relies on C23 enums */
    _Static_assert(((enum rank_index)-1) > 0, "rank_index must be unsigned");
    _Static_assert(((enum file_index)-1) > 0, "file_index must be unsigned");


    enum rank_index walk_rank;
    enum file_index walk_file;
    for (walk_rank = rank+1, walk_file = file+1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            ++walk_rank, ++walk_file) {
        bitboard const sq = SQ_MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (walk_rank = rank+1, walk_file = file-1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            ++walk_rank, --walk_file) {
        bitboard const sq = SQ_MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (walk_rank = rank-1, walk_file = file+1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            --walk_rank, ++walk_file) {
        bitboard const sq = SQ_MASK_FROM_RF(walk_rank, walk_file);
        atk |= sq;
        if (occ & sq) {
            break;
        }
    }
    for (walk_rank = rank-1, walk_file = file-1;
            walk_rank <= RANK_INDEX_8 && walk_file <= FILE_INDEX_H;
            --walk_rank, --walk_file) {
        bitboard const sq = SQ_MASK_FROM_RF(walk_rank, walk_file);
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
   - `mbb_bishop[SQ_INDEX_COUNT]`
   - `bitboard bishop_attacks[SQ_INDEX_COUNT][1<<9ULL] */
#include "mbb_bishop.h"
    struct magic const m = mbb_bishop[sq];
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
        index const lsb = bitboard_pop_lsb(&p);
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
        index const lsb = bitboard_pop_lsb(&p);
        b |= queen_attacks_from_index(lsb, occ);
    }
    return b;
}

static inline bitboard pawn_moves_white(bitboard p, bitboard empty)
{
    /* Assumptions:
       - empty = ~(black_occupied | white_occupied);
     */

    bitboard const single_push = RANK_SHIFT_UP_1(p) & empty;

    bitboard const double_push =
        RANK_SHIFT_UP_1(single_push & RANK_MASK_3) & empty;

    return (single_push | double_push);
}

static inline bitboard pawn_attacks_white(bitboard p)
{
    bitboard const up = RANK_SHIFT_UP_1(p);
    bitboard const captures_right = FILE_SHIFT_RIGHT_1(up);
    bitboard const captures_left  = FILE_SHIFT_LEFT_1(up);

    return (captures_right | captures_left);
}

static inline bitboard pawn_moves_black(bitboard p, bitboard empty)
{
    /* Assumptions:
       - empty = ~(black_occupied | white_occupied);
     */

    bitboard const single_push = RANK_SHIFT_DOWN_1(p) & empty;

    bitboard const double_push =
        RANK_SHIFT_DOWN_1(single_push & RANK_MASK_6) & empty;

    return (single_push | double_push);
}

static inline bitboard pawn_attacks_black(bitboard p)
{
    bitboard const down = RANK_SHIFT_DOWN_1(p);
    bitboard const captures_right = FILE_SHIFT_RIGHT_1(down);
    bitboard const captures_left  = FILE_SHIFT_LEFT_1(down);

    return (captures_right | captures_left);
}

/* temporary solution, annoying branch */
static inline bitboard pawn_moves_dispatch(bitboard p, bitboard empty, enum player attacker)
{
    if (attacker == PLAYER_WHITE) {
        return pawn_moves_white(p, empty);
    } else {
        return pawn_moves_black(p, empty);
    }
}

/* temporary solution, annoying branch */
static inline bitboard pawn_attacks_dispatch(bitboard p, enum player attacker)
{
    if (attacker == PLAYER_WHITE) {
        return pawn_attacks_white(p);
    } else {
        return pawn_attacks_black(p);
    }
}

static bitboard king_attacks(bitboard sq)
{
    /* potential untested improvements:
     * - lookup table
     * - union of three files, three ranks and ~sq */
    bitboard const n = RANK_SHIFT_UP_1(sq);
    bitboard const s = RANK_SHIFT_DOWN_1(sq);
    bitboard const w = FILE_SHIFT_LEFT_1(sq);
    bitboard const e = FILE_SHIFT_RIGHT_1(sq);
                   
    bitboard const nw = FILE_SHIFT_LEFT_1(n);
    bitboard const ne = FILE_SHIFT_RIGHT_1(n);
    bitboard const sw = FILE_SHIFT_LEFT_1(s);
    bitboard const se = FILE_SHIFT_RIGHT_1(s);

    return (n | s | w | e | nw | ne | sw | se);
}

static bitboard piece_attacks(enum piece  piece,
                              enum player player,
                              bitboard    ep_targets,
                              bitboard    p,
                              bitboard    occ,
                              bitboard    their_occ)
{
    switch (piece) {
        case PIECE_PAWN: {
            return pawn_attacks_dispatch(p, player) & (their_occ | ep_targets);
        } break;

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

#ifndef NDEBUG
        default: {
            __builtin_trap();
            __builtin_unreachable();
        } break;
#endif
    }
}

static bitboard piece_moves(enum piece  piece,
                            enum player player,
                            bitboard    ep_targets,
                            bitboard    p,
                            bitboard    occ,
                            bitboard    their_occ)
{
    bitboard x = 0ULL;
    if (piece == PIECE_PAWN) {
        x |= pawn_moves_dispatch(p, ~occ, player);
    }
    x |= piece_attacks(piece, player, ep_targets, p, occ, their_occ);
    return x;
}

/* BOARD
 * ================= */
enum castle_direction {
    CASTLE_BEGIN,
    CASTLE_KINGSIDE = CASTLE_BEGIN,
    CASTLE_QUEENSIDE,
    CASTLE_COUNT,
};

enum {
    MATTR_CAPTURE          = 1<<0,
    MATTR_PROMOTE          = 1<<1,
    MATTR_CHECK            = 1<<2,
    MATTR_CASTLE_KINGSIDE  = 1<<3,
    MATTR_CASTLE_QUEENSIDE = 1<<4,
};

struct move {
    index   from;
    index   to;
    uint8_t attr;
    #define APPEAL_MAX 127
    int8_t  appeal;
};

enum  {
    SO_ATTR_CHECK = 1<<0,
};

struct search_option {
    /* TODO: optimize field order */
    double      score;
    struct move move;
    uint64_t    hash;
    int8_t      depth;
    uint8_t     init;
    enum tt_flag {TT_EXACT, TT_LOWER, TT_UPPER} flag;
    uint16_t attr;
};

#define TT_ADDRESS_BITS 24
#define TT_ENTRIES (1ULL<<TT_ADDRESS_BITS)
struct tt {
    struct search_option* entries; /* must be malloc'ed or mmap'ed */
};


struct board {
    struct pos {
        bitboard pieces[PLAYER_COUNT][PIECE_COUNT];
        enum player player;
        bool castling_illegal[PLAYER_COUNT][CASTLE_COUNT];
        bitboard ep_targets;
        bitboard occupied[PLAYER_COUNT];

        int halfmoves; /* FIXME: this duplicates the hist.length value */
        int fullmoves;
        uint64_t hash;
    } pos;

    struct tt tt;

    /* used for repeated board state detection only */
    struct history {
        struct pos items[4096];
        size_t length;
    } hist;

    /* Used only for square->piece lookups, not central to logic.  Does not
     * store which player owns the square piece, and checking non-occupied
     * squares is undefined
     *
     * TODO: make mailbox smaller, only 3 bits are needed per piece
     * */
    enum piece mailbox[SQ_INDEX_COUNT];
};

static void move_compute_appeal(struct move*      m,
                                struct pos const* pos,
                                enum player       us,
                                enum piece        mailbox[restrict static SQ_INDEX_COUNT])
{
    enum player them = opposite_player(us);
    /* MVV-LVA: https://www.chessprogramming.org/MVV-LVA */
    enum piece const atk = mailbox[m->from];

    int8_t n = 1;
    if (SQ_MASK_FROM_INDEX(m->to) & pos->occupied[them]) {
        n += (int8_t)piece_value[mailbox[m->to]];
    }

    m->appeal = 16*(int8_t)n - (int8_t)piece_value[atk];
}

#define BOARD_INITIAL (struct board) {                    \
    .pos = {                                              \
        .fullmoves = 1,                                   \
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
        .hash = ~0ULL,                                    \
    },                                                    \
    .mailbox = {                                          \
        [SQ_INDEX_A1] = PIECE_ROOK,                       \
        [SQ_INDEX_B1] = PIECE_KNIGHT,                     \
        [SQ_INDEX_C1] = PIECE_BISHOP,                     \
        [SQ_INDEX_D1] = PIECE_QUEEN,                      \
        [SQ_INDEX_E1] = PIECE_KING,                       \
        [SQ_INDEX_F1] = PIECE_BISHOP,                     \
        [SQ_INDEX_G1] = PIECE_KNIGHT,                     \
        [SQ_INDEX_H1] = PIECE_ROOK,                       \
        [SQ_INDEX_A2] = PIECE_PAWN,                       \
        [SQ_INDEX_B2] = PIECE_PAWN,                       \
        [SQ_INDEX_C2] = PIECE_PAWN,                       \
        [SQ_INDEX_D2] = PIECE_PAWN,                       \
        [SQ_INDEX_E2] = PIECE_PAWN,                       \
        [SQ_INDEX_F2] = PIECE_PAWN,                       \
        [SQ_INDEX_G2] = PIECE_PAWN,                       \
        [SQ_INDEX_H2] = PIECE_PAWN,                       \
        [SQ_INDEX_A7] = PIECE_PAWN,                       \
        [SQ_INDEX_B7] = PIECE_PAWN,                       \
        [SQ_INDEX_C7] = PIECE_PAWN,                       \
        [SQ_INDEX_D7] = PIECE_PAWN,                       \
        [SQ_INDEX_E7] = PIECE_PAWN,                       \
        [SQ_INDEX_F7] = PIECE_PAWN,                       \
        [SQ_INDEX_G7] = PIECE_PAWN,                       \
        [SQ_INDEX_H7] = PIECE_PAWN,                       \
        [SQ_INDEX_A8] = PIECE_ROOK,                       \
        [SQ_INDEX_B8] = PIECE_KNIGHT,                     \
        [SQ_INDEX_C8] = PIECE_BISHOP,                     \
        [SQ_INDEX_D8] = PIECE_QUEEN,                      \
        [SQ_INDEX_E8] = PIECE_KING,                       \
        [SQ_INDEX_F8] = PIECE_BISHOP,                     \
        [SQ_INDEX_G8] = PIECE_KNIGHT,                     \
        [SQ_INDEX_H8] = PIECE_ROOK,                       \
    },                                                    \
    .hist = {0},                                          \
}

static void board_print_fen(struct board const* b, FILE* out)
{
    int buf[8][8] = {0};

    for (enum player player = PLAYER_BEGIN; player < PLAYER_COUNT; ++player) {
        for (enum piece piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
            bitboard x = b->pos.pieces[player][piece];

            for (index i = 7; i < 8; i--) {
                for (index j = 0; j < 8; ++j) {
                    if (x & (1ULL<<(i*8+j))) {
                        buf[i][j] = piece_char[player][piece];
                    }
                }
            }
        }
    }

    for (enum rank_index i = RANK_INDEX_8; i < RANK_INDEX_COUNT; --i) {
        int consequtive_empty = 0;
        for (enum file_index j = FILE_INDEX_A; j < FILE_INDEX_COUNT; ++j) {
            if (buf[i][j]) {
                if (consequtive_empty) {
                    fprintf(out, "%d", consequtive_empty);
                    consequtive_empty = 0;
                }
                fprintf(out, "%lc", buf[i][j]);
            } else {
                consequtive_empty += 1;
            }
        }
        if (consequtive_empty) {
            fprintf(out, "%d", consequtive_empty);
        }
        if (i > 0)
            fprintf(out, "/");
    }

    fprintf(out, " %c ", b->pos.player == PLAYER_WHITE ? 'w' : 'b');

    bool any_castle = false;
    if (!b->pos.castling_illegal[PLAYER_WHITE][CASTLE_KINGSIDE]) {
        fprintf(out, "K");
        any_castle = true;
    }
    if (!b->pos.castling_illegal[PLAYER_WHITE][CASTLE_QUEENSIDE]) {
        fprintf(out, "Q");
        any_castle = true;
    }
    if (!b->pos.castling_illegal[PLAYER_BLACK][CASTLE_KINGSIDE]) {
        fprintf(out, "k");
        any_castle = true;
    }
    if (!b->pos.castling_illegal[PLAYER_BLACK][CASTLE_QUEENSIDE]) {
        fprintf(out, "q");
        any_castle = true;
    }
    if (!any_castle) {
        fprintf(out, "-");
    }

    if (b->pos.ep_targets) {
        /* should be ep target square in algebraic notation */
        fprintf(stderr, "not implemented: fen with en passent squares\n");
        fprintf(out, "<TODO>");
    } else {
        fprintf(out, " -");
    }

    fprintf(out, " %d", b->pos.halfmoves);
    fprintf(out, " %d", b->pos.fullmoves);

    fprintf(out, "\n");
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
#define BUF_GETCHAR(x) (fprintf(stderr, "[%c]", x.buf[x.i]), assert(x.i < x.len), x.buf[x.i++])

    memset(&b->pos, 0, sizeof b->pos);
    b->pos.hash = ~0ULL;

    for (enum rank_index ri = RANK_INDEX_8; ri < RANK_INDEX_COUNT; --ri) {
        for (enum file_index fi = FILE_INDEX_BEGIN; fi < FILE_INDEX_COUNT; ++fi) {
            char const ch = BUF_GETCHAR(fen);
            if (isdigit(ch)) {
                if (ch == '0') {
                    abort();
                }
                fi += ch - '0' - 1;
            } else {
                struct piece_player const p = piece_and_player_from_char[(uint8_t)ch];
                bitboard const sq_mask = SQ_MASK_FROM_RF(ri, fi);
                b->pos.pieces[p.player][p.piece] |= sq_mask;
                b->pos.occupied[p.player] |= sq_mask;
                b->mailbox[INDEX_FROM_RF(ri, fi)] = p.piece;
            }
        }
        (void)BUF_GETCHAR(fen); /* discard '/' or ' ' */
    }

    { /* active color */
        char const ch = BUF_GETCHAR(fen);
        if (ch == 'w') {
            b->pos.player = PLAYER_WHITE;
        } else if (ch == 'b') {
            b->pos.player = PLAYER_BLACK;
        } else {
            abort();
        }
    }

    { /* castling */
        char ch = BUF_GETCHAR(fen);
        if (ch != ' ') {
            abort();
        }
        b->pos.castling_illegal[PLAYER_WHITE][CASTLE_KINGSIDE] = true;
        b->pos.castling_illegal[PLAYER_WHITE][CASTLE_QUEENSIDE] = true;
        b->pos.castling_illegal[PLAYER_BLACK][CASTLE_KINGSIDE] = true;
        b->pos.castling_illegal[PLAYER_BLACK][CASTLE_QUEENSIDE] = true;
        do {
            ch = BUF_GETCHAR(fen);
            switch (ch) {
                case 'K': b->pos.castling_illegal[PLAYER_WHITE][CASTLE_KINGSIDE] = false;  break;
                case 'Q': b->pos.castling_illegal[PLAYER_WHITE][CASTLE_QUEENSIDE] = false; break;
                case 'k': b->pos.castling_illegal[PLAYER_BLACK][CASTLE_KINGSIDE] = false;  break;
                case 'q': b->pos.castling_illegal[PLAYER_BLACK][CASTLE_QUEENSIDE] = false; break;
                case ' ': break;
                case '-': break;
                default: {
                    fprintf(stderr, "unexpected char '%c'\n", ch); 
                    abort();
                } break;
            }
        } while (ch != ' ');
    }

    { /* en passent */
        char const ch = BUF_GETCHAR(fen);
        if (ch != '-') {
            fprintf(stderr, "ep targets in fen not implemented yet\n");
            abort();
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

static bitboard all_threats_from_player(struct pos const * pos, enum player player)
{
    bitboard const occ = pos->occupied[PLAYER_WHITE] | pos->occupied[PLAYER_BLACK];

    bitboard const* p = pos->pieces[player];
    bitboard t = 0ULL;
    t |= pawn_attacks_dispatch(p[PIECE_PAWN], player);
    t |= knight_attacks(p[PIECE_KNIGHT]);
    t |= bishop_attacks(p[PIECE_BISHOP], occ);
    t |= rook_attacks(p[PIECE_ROOK], occ);
    t |= queen_attacks(p[PIECE_QUEEN], occ);
    t |= king_attacks(p[PIECE_KING]);
    return t;
}

static bitboard between_mask(enum square_index a, enum square_index b)
{
#ifdef CODEGEN
    enum rank_index const ra = index_to_rank(a);
    enum file_index const fa = index_to_file(a);
    enum rank_index const rb = index_to_rank(b);
    enum file_index const fb = index_to_file(b);

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

    bitboard mask = 0ULL;

    enum rank_index r = (enum rank_index)((int)ra + step_r);
    enum file_index f = (enum file_index)((int)fa + step_f);

    while (r != rb || f != fb) {
        mask |= SQ_MASK_FROM_RF(r, f);
        r = (enum rank_index)((int)r + step_r);
        f = (enum file_index)((int)f + step_f);
    }

    return mask;
#else
    #if ! __has_include("between_lookup.h")
        #error "compile codegen.c and run once to generate required header files"
    #else

    /* defines static between_lookup[SQUARE_INDEX_COUNT][SQUARE_INDEX_COUNT] */
    #include "between_lookup.h"
    return between_lookup[a][b];
    #endif
#endif
}

static bitboard attacks_to(struct pos const* pos,
                           bitboard target_square,
                           bitboard pretend_occupied,
                           bitboard pretend_empty)
{
    bitboard const occ_orig = pos->occupied[PLAYER_WHITE] | pos->occupied[PLAYER_BLACK];
    bitboard const occ = (occ_orig & ~pretend_empty) | pretend_occupied;

    bitboard const* pw = pos->pieces[PLAYER_WHITE];
    bitboard const* pb = pos->pieces[PLAYER_BLACK];

    bitboard const wps = pw[PIECE_PAWN];

    bitboard const bps = pb[PIECE_PAWN];

    bitboard const ns = (pw[PIECE_KNIGHT]
                      |  pb[PIECE_KNIGHT])
                      ;
    bitboard const ks = (pw[PIECE_KING]
                      |  pb[PIECE_KING])
                      ;
    bitboard const qs = (pw[PIECE_QUEEN]
                      |  pb[PIECE_QUEEN])
                      ;
    bitboard const qrs = (pw[PIECE_ROOK]
                       |  pb[PIECE_ROOK]
                       |  qs)
                       ;
    bitboard const qbs = (pw[PIECE_BISHOP]
                       |  pb[PIECE_BISHOP]
                       |  qs)
                       ;

    enum square_index target_index = bitboard_lsb(target_square);

    return ((bps & pawn_attacks_white(target_square))
          | (wps & pawn_attacks_black(target_square))
          | (ns & knight_attacks(target_square))
          | (qbs & bishop_attacks_from_index(target_index, occ))
          | (qrs & rook_attacks_from_index(target_index, occ))
          | (ks & king_attacks(target_square)))
          & ~pretend_occupied;
}

static bitboard attacks_to_king(struct pos const* pos,
                                bitboard target_square,
                                bitboard pretend_occupied,
                                bitboard pretend_empty)
{
    bitboard const occ_orig = pos->occupied[PLAYER_WHITE] | pos->occupied[PLAYER_BLACK];
    bitboard const occ = (occ_orig & ~pretend_empty) | pretend_occupied;

    bitboard const* pw = pos->pieces[PLAYER_WHITE];
    bitboard const* pb = pos->pieces[PLAYER_BLACK];

    bitboard const wps = pw[PIECE_PAWN];

    bitboard const bps = pb[PIECE_PAWN];

    bitboard const ns = (pw[PIECE_KNIGHT] |  pb[PIECE_KNIGHT]);
    bitboard const qs = (pw[PIECE_QUEEN] |  pb[PIECE_QUEEN]);

    bitboard const qrs = (pw[PIECE_ROOK]   |  pb[PIECE_ROOK]   |  qs) ;
    bitboard const qbs = (pw[PIECE_BISHOP] |  pb[PIECE_BISHOP] |  qs) ;

    enum square_index target_index = bitboard_lsb(target_square);

    return ((bps & pawn_attacks_white(target_square))
          | (wps & pawn_attacks_black(target_square))
          | (ns & knight_attacks(target_square))
          | (qbs & bishop_attacks_from_index(target_index, occ))
          | (qrs & rook_attacks_from_index(target_index, occ)))
          & ~pretend_occupied;
}

_Static_assert(sizeof(struct move) == 4,
        "this static assert is here to check when sizeof(move) changes");

#define MOVE_CASTLE_KINGSIDE_WHITE (struct move) \
    {.from = SQ_INDEX_E1, .to = SQ_INDEX_G1, .attr = MATTR_CASTLE_KINGSIDE}

#define MOVE_CASTLE_QUEENSIDE_WHITE \
    (struct move){.from = SQ_INDEX_E1, .to = SQ_INDEX_C1, .attr = MATTR_CASTLE_QUEENSIDE}

#define MOVE_CASTLE_KINGSIDE_BLACK (struct move) \
    {.from = SQ_INDEX_E8, .to = SQ_INDEX_G8, .attr = MATTR_CASTLE_KINGSIDE}

#define MOVE_CASTLE_QUEENSIDE_BLACK \
    (struct move){.from = SQ_INDEX_E8, .to = SQ_INDEX_C8, .attr = MATTR_CASTLE_QUEENSIDE}

struct move move_make(struct pos const* pos, enum piece piece, index from, index to, uint8_t add_attr)
{
    enum player const us = pos->player;
    enum player const them = opposite_player(us);
    bitboard const their_occ = pos->occupied[them];
    bitboard const tomask = SQ_MASK_FROM_INDEX(to);
    bitboard const finishline = (us == PLAYER_WHITE ? RANK_MASK_8 : RANK_MASK_1);

    uint8_t attr = 0ULL;
#define MASK_IF8(x) ((~(uint8_t)0U) + (uint8_t)!(x))
    attr |= MATTR_CAPTURE & MASK_IF8(tomask & their_occ);
    attr |= MATTR_CAPTURE & MASK_IF8((piece == PIECE_PAWN) && tomask & pos->ep_targets);
    attr |= MATTR_PROMOTE & MASK_IF8((piece == PIECE_PAWN) && (tomask & finishline));
    attr |= add_attr;
#undef MASK_IF8

    return (struct move){.from = from, .to = to, .attr = attr};
}

#define MOVE_MAX 128

static void all_moves_from_piece(struct pos const* restrict pos,
                                 enum piece                 piece,
                                 enum player                us,
                                 enum square_index          from,
                                 bitboard                   allowed,
                                 size_t* restrict           out_count,
                                 struct move                out[restrict static MOVE_MAX])
{
    /* TODO: this function is terrible, needs major rework
     *
     * instead of dispatching to a generalized `piece_moves`, this can be specialized for each
     * piece
     * */
    assert(piece != PIECE_KING);
    enum player const them = opposite_player(us);

    bitboard const our_occ   = pos->occupied[us];
    bitboard const their_occ = pos->occupied[them];
    bitboard const all_occ   = pos->occupied[PLAYER_WHITE] | pos->occupied[PLAYER_BLACK];
                   
    bitboard const from_mask = SQ_MASK_FROM_INDEX(from);

    bitboard move_mask =
          piece_moves(piece, us, pos->ep_targets, from_mask, all_occ, their_occ)
        & ~our_occ
        /*& allowed*/
        ;

    while (move_mask) {
        enum square_index const to = bitboard_pop_lsb(&move_mask);
        bitboard const to_mask = SQ_MASK_FROM_INDEX(to);

        bitboard const king_attackers = 
            attacks_to(pos, pos->pieces[us][PIECE_KING], to_mask, from_mask) & ~(our_occ | to_mask);

        if (king_attackers == 0ULL) {
            out[(*out_count)++] = move_make(pos, piece, from, to, 0);
        }
    }
}

static void all_moves_from_king(struct pos const* restrict pos,
                                enum player                us,
                                enum square_index          from,
                                size_t* restrict           out_count,
                                struct move                out[restrict static MOVE_MAX])
{
    enum player const them = opposite_player(us);
    bitboard const our_occ = pos->occupied[us];
    bitboard const all_occ = pos->occupied[PLAYER_WHITE] | pos->occupied[PLAYER_BLACK];

    bitboard const from_mask = SQ_MASK_FROM_INDEX(from);
    bitboard const threats = all_threats_from_player(pos, them);

    bitboard move_mask = king_attacks(from_mask) & ~(threats | our_occ);

    /* general moves */
    while (move_mask) {
        enum square_index const to = bitboard_pop_lsb(&move_mask);
        bitboard const to_mask = SQ_MASK_FROM_INDEX(to);

        bitboard const king_attackers = attacks_to(pos, to_mask, to_mask, from_mask)  & ~(our_occ | to_mask);

        if (king_attackers == 0ULL) {
            out[(*out_count)++] = move_make(pos, PIECE_KING, from, to, 0);
        }
    }

    /* castling */
    if (from_mask & ~threats) {
        bool kingside_free, queenside_free;
        if (us == PLAYER_WHITE) {
            kingside_free  = !((threats | all_occ) & (SQ_MASK_F1 | SQ_MASK_G1))
                          && (pos->pieces[us][PIECE_ROOK] & SQ_MASK_H1);
            queenside_free = !((threats | all_occ) & (SQ_MASK_B1 | SQ_MASK_C1 | SQ_MASK_D1))
                          && (pos->pieces[us][PIECE_ROOK] & SQ_MASK_A1);
        } else {
            kingside_free  = !((threats | all_occ) & (SQ_MASK_F8 | SQ_MASK_G8))
                          && (pos->pieces[us][PIECE_ROOK] & SQ_MASK_H8);
            queenside_free = !((threats | all_occ) & (SQ_MASK_B8 | SQ_MASK_C8 | SQ_MASK_D8))
                          && (pos->pieces[us][PIECE_ROOK] & SQ_MASK_A8);
        }
        if (!pos->castling_illegal[us][CASTLE_KINGSIDE] && kingside_free)
        {
            out[(*out_count)++] = us == PLAYER_WHITE 
                                  ? MOVE_CASTLE_KINGSIDE_WHITE
                                  : MOVE_CASTLE_KINGSIDE_BLACK;
        }
        if (!pos->castling_illegal[us][CASTLE_QUEENSIDE] && queenside_free)
        {
            out[(*out_count)++] = us == PLAYER_WHITE 
                                  ? MOVE_CASTLE_QUEENSIDE_WHITE
                                  : MOVE_CASTLE_QUEENSIDE_BLACK;
        }
    }
}

static void all_moves(struct pos const* restrict pos,
                      enum player                us,
                      size_t* restrict           out_count,
                      struct move                out[restrict static MOVE_MAX])
{
    *out_count = 0ULL;
    bitboard const myking = pos->pieces[us][PIECE_KING];
    assert(myking);

    enum square_index const myking_index = bitboard_lsb(myking);
    bitboard const attackers = attacks_to_king(pos, myking, 0ULL, 0ULL) & ~pos->occupied[us];


    bitboard allowed = ~0ULL;

    uint64_t const natk = bitboard_popcount(attackers);
    //if (natk == 1) {
    //    enum square_index const attacker_index = bitboard_lsb(attackers);
    //    allowed = between_mask(myking_index, attacker_index) | attackers;
    //} else {
    //    bitboard const pinners = pinning_lines(pos, us, myking_index);
    //}

    /* king */
    {
        all_moves_from_king(pos, us, myking_index, out_count, out);
    }

    if (natk >= 2) {
        /* no other piece can fix check if the king has two attackers or more */
        return;
    }

    /* pawns */
    {
        bitboard ps = pos->pieces[us][PIECE_PAWN];
        while (ps) {
            enum square_index const from = bitboard_pop_lsb(&ps);
            all_moves_from_piece(pos, PIECE_PAWN, us, from, allowed, out_count, out);
        }
    }

    /* knights */
    {
        bitboard ns = pos->pieces[us][PIECE_KNIGHT];
        while (ns) {
            index const from = bitboard_pop_lsb(&ns);
            all_moves_from_piece(pos, PIECE_KNIGHT, us, from, allowed, out_count, out);
        }
    }

    /* bishops */
    {
        bitboard bs = pos->pieces[us][PIECE_BISHOP];
        while (bs) {
            index const from = bitboard_pop_lsb(&bs);
            all_moves_from_piece(pos, PIECE_BISHOP, us, from, allowed, out_count, out);
        }
    }

    /* rooks */
    {
        bitboard rs = pos->pieces[us][PIECE_ROOK];
        while (rs) {
            index const from = bitboard_pop_lsb(&rs);
            all_moves_from_piece(pos, PIECE_ROOK, us, from, allowed, out_count, out);
        }
    }

    /* queens */
    {
        bitboard qs = pos->pieces[us][PIECE_QUEEN];
        while (qs) {
            index const from = bitboard_pop_lsb(&qs);
            all_moves_from_piece(pos, PIECE_QUEEN, us, from, allowed, out_count, out);
        }
    }
}

struct zobrist {
    uint64_t piece_keys[SQ_INDEX_COUNT][PLAYER_COUNT][PIECE_COUNT];
    uint64_t ep_targets[SQ_INDEX_COUNT];
    uint64_t castling_keys[PLAYER_COUNT][CASTLE_COUNT];
    bool init;
};

static struct zobrist zobrist;

static void init_zobrist()
{
    for (enum square_index sq = SQ_INDEX_BEGIN; sq < SQ_INDEX_COUNT; ++sq) {
        for (enum player pl = PLAYER_BEGIN; pl < PLAYER_COUNT; ++pl) {
            for (enum piece piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
                zobrist.piece_keys[sq][pl][piece] = rand64();
            }
        }
        zobrist.ep_targets[sq] = rand64();
    }
    for (enum player pl = PLAYER_BEGIN; pl < PLAYER_COUNT; ++pl) {
        for (enum castle_direction d = CASTLE_BEGIN; d < CASTLE_COUNT; ++d) {
            zobrist.castling_keys[pl][d] = rand64();
        }
    }
    zobrist.init = true;
}

static inline uint64_t tt_hash_update(uint64_t          hash,
                                      enum square_index sq,
                                      enum player       us,
                                      enum piece        piece)
{
    if (!zobrist.init) {
        init_zobrist();
    }
    return hash ^ zobrist.piece_keys[sq][us][piece];
}

static inline uint64_t tt_hash_update_castling_rights(uint64_t              hash,
                                                      enum player           us,
                                                      enum castle_direction dir)
{
    if (!zobrist.init) {
        init_zobrist();
    }
    return hash ^ zobrist.castling_keys[us][dir];
}

static inline uint64_t tt_hash_update_ep_targets(uint64_t hash, enum square_index sq)
{
    if (!zobrist.init) {
        init_zobrist();
    }
    assert(sq < SQ_INDEX_COUNT);
    return hash ^ zobrist.ep_targets[sq];
}

static inline uint64_t tt_hash_switch_player(uint64_t hash)
{
    if (!zobrist.init) {
        init_zobrist();
    }
    return ~hash;
}

static inline struct search_option tt_get(struct tt const* tt, uint64_t hash)
{
    uint64_t const addr = hash % TT_ENTRIES; 
    return tt->entries[addr];
}

static inline void tt_insert(struct tt* tt, uint64_t hash, struct search_option so)
{
    uint64_t const addr = hash % TT_ENTRIES; 
    so.init = true;
    tt->entries[addr] = so;
}

enum move_result {
    MR_NORMAL,
    MR_CHECK,
    MR_STALEMATE,
    MR_CHECKMATE,
};

/* does not check validity */
static enum move_result board_move(struct pos* restrict     pos,
                                   struct history* restrict hist,
                                   enum piece               mailbox[restrict static SQ_INDEX_COUNT],
                                   struct move              move)
{
    enum player const us   = pos->player;
    enum player const them = opposite_player(us);

    enum piece const from_piece = mailbox[move.from];
    enum piece const to_piece = mailbox[move.to];

    bitboard const from_mask = SQ_MASK_FROM_INDEX(move.from);
    bitboard const to_mask   = SQ_MASK_FROM_INDEX(move.to);

    enum square_index const krook       = (us == PLAYER_WHITE) ? SQ_INDEX_H1 : SQ_INDEX_H8;
    enum square_index const qrook       = (us == PLAYER_WHITE) ? SQ_INDEX_A1 : SQ_INDEX_A8;
    enum square_index const krook_to    = (us == PLAYER_WHITE) ? SQ_INDEX_F1 : SQ_INDEX_F8;
    enum square_index const qrook_to    = (us == PLAYER_WHITE) ? SQ_INDEX_D1 : SQ_INDEX_D8;
    enum square_index const ksq         = (us == PLAYER_WHITE) ? SQ_INDEX_E1 : SQ_INDEX_E8;
    enum square_index const kcast_sq    = (us == PLAYER_WHITE) ? SQ_INDEX_G1 : SQ_INDEX_G8;
    enum square_index const qcast_sq    = (us == PLAYER_WHITE) ? SQ_INDEX_C1 : SQ_INDEX_C8;
    enum square_index const their_krook = (us != PLAYER_WHITE) ? SQ_INDEX_H1 : SQ_INDEX_H8;
    enum square_index const their_qrook = (us != PLAYER_WHITE) ? SQ_INDEX_A1 : SQ_INDEX_A8;

    #define POS_MOVE(player, piece, from, to) \
        do { \
            bitboard const x = SQ_MASK_FROM_INDEX(from) | SQ_MASK_FROM_INDEX(to); \
            pos->pieces[player][piece] ^= x; \
            pos->occupied[player] ^= x; \
            pos->hash = tt_hash_update(pos->hash, from, player, piece); \
            pos->hash = tt_hash_update(pos->hash, to, player, piece); \
            mailbox[to] = piece; \
            if (piece == PIECE_PAWN) pos->halfmoves = 0; \
        } while (0)

    #define POS_REMOVE(owner, piece, at) \
        do { \
            bitboard const x = SQ_MASK_FROM_INDEX(at); \
            pos->pieces[owner][piece] &= ~x; \
            pos->occupied[owner] &= ~x; \
            pos->hash = tt_hash_update(pos->hash, at, owner, piece); \
            hist->length = 0; \
            pos->halfmoves = 0; \
        } while (0)

    #define POS_ADD(owner, piece, at) \
        do { \
            bitboard const x = SQ_MASK_FROM_INDEX(at); \
            pos->pieces[owner][piece] |= x; \
            pos->occupied[owner] |= x; \
            pos->hash = tt_hash_update(pos->hash, at, owner, piece); \
            pos->halfmoves = 0; \
            hist->length = 0; \
        } while (0)

    bitboard const ep_targets_now = pos->ep_targets;
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
        /**/ if (to_mask & pos->occupied[them]) {
            POS_REMOVE(them, to_piece, move.to);
        }
        else if (from_piece == PIECE_PAWN) {
            bitboard const finishline = (us == PLAYER_WHITE ? RANK_MASK_8 : RANK_MASK_1);

            /* en passent */
            /**/ if (to_mask & ep_targets_now) {
                enum square_index const ti =
                    (us == PLAYER_WHITE)
                    ? INDEX_SHIFT_DOWN(move.to, 1)
                    : INDEX_SHIFT_UP(move.to, 1);
                POS_REMOVE(them, PIECE_PAWN, ti);
            }
            /* pawn double push -> new ep_target */
            else if (us == PLAYER_WHITE && (from_mask & RANK_MASK_2) && (to_mask & RANK_MASK_4)) {
                pos->ep_targets |= RANK_SHIFT_UP_1(from_mask);
                pos->hash = tt_hash_update_ep_targets(pos->hash, INDEX_SHIFT_UP(move.from, 1));
            }
            else if (us == PLAYER_BLACK && (from_mask & RANK_MASK_7) && (to_mask & RANK_MASK_5)) {
                pos->ep_targets |= RANK_SHIFT_DOWN_1(from_mask);
                pos->hash = tt_hash_update_ep_targets(pos->hash, INDEX_SHIFT_DOWN(move.from, 1));
            }
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

    pos->hash = tt_hash_switch_player(pos->hash);
    pos->player = them;
    pos->fullmoves += (pos->player == PLAYER_BLACK);
    pos->halfmoves += 1;

    assert(hist->length < 4096);
    for (size_t i = 0; i < hist->length; ++i) {
        _Static_assert(sizeof *pos == sizeof hist->items[i]);
        if (!memcmp(&hist->items[i].pieces, &pos->pieces, sizeof pos->pieces)
         && !memcmp(&hist->items[i].castling_illegal, &pos->castling_illegal, sizeof pos->castling_illegal)
         && hist->items[i].player == pos->player
         && hist->items[i].ep_targets == pos->ep_targets)
        {
            return MR_STALEMATE;
        }
    }
    hist->items[hist->length++] = *pos;

    if (pos->halfmoves > 50) {
        return MR_STALEMATE;
    }

    if (attacks_to_king(pos, pos->pieces[them][PIECE_KING], 0ULL, 0ULL) & ~pos->occupied[them]) {
        return MR_CHECK;
    } else {
        return MR_NORMAL;
    }
}



static enum move_result board_move_2(struct board* b, struct move move)
{
    return board_move(&b->pos,
                      &b->hist,
                      b->mailbox,
                      move);
}

/*
 * Placeholder heuristic
 *
 * */
static double board_score_heuristic(struct pos const* pos)
{
    double score = 0.0;

    #define BOARD_CENTER ((FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F) \
        & ~(RANK_MASK_1 | RANK_MASK_2 | RANK_MASK_7 | RANK_MASK_8))
    static bitboard const positional_modifier_area[PIECE_COUNT] = {
        [PIECE_PAWN] = BOARD_CENTER,
        [PIECE_KNIGHT] = BOARD_CENTER,
        [PIECE_QUEEN] = RANK_MASK_3 | RANK_MASK_4 | RANK_MASK_5 | RANK_MASK_6,
    };
    #undef BOARD_CENTER
    static double const positional_modifier_factor[PIECE_COUNT] = {
        [PIECE_PAWN]   =  0.02,
        [PIECE_KNIGHT] =  0.02,
        [PIECE_QUEEN]  = -0.03,
    };

    bitboard const white_threats = all_threats_from_player(pos, PLAYER_WHITE);
    bitboard const black_threats = all_threats_from_player(pos, PLAYER_BLACK);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
    for (enum piece p = PIECE_BEGIN; p < PIECE_COUNT; ++p) {
        score += 0.002*piece_value[p] * 
            ((double)bitboard_popcount(white_threats & pos->pieces[PLAYER_BLACK][p]) -
             (double)bitboard_popcount(black_threats & pos->pieces[PLAYER_WHITE][p]));

        score += piece_value[p] *
            ((double)bitboard_popcount(pos->pieces[PLAYER_WHITE][p]) -
             (double)bitboard_popcount(pos->pieces[PLAYER_BLACK][p]));

        score += positional_modifier_factor[p] *
            ((double)bitboard_popcount(pos->pieces[PLAYER_WHITE][p] & positional_modifier_area[p]) -
             (double)bitboard_popcount(pos->pieces[PLAYER_BLACK][p] & positional_modifier_area[p]));
    }
#pragma clang diagnostic pop

    double sign = (pos->player == PLAYER_WHITE) ? 1.0 : -1.0;

    return sign*score;
}

static inline struct move moves_linear_search(struct move      moves[restrict static MOVE_MAX],
                                              size_t* restrict move_count)
{
    size_t best = 0;
    assert(*move_count > 0);
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
double quiesce(struct pos const* pos,
               enum piece        mailbox[restrict static SQ_INDEX_COUNT],
               enum player       us,
               double            alpha,
               double            beta,
               int8_t            depth)
{
    enum player const them = opposite_player(us);

    double score = board_score_heuristic(pos);
    double highscore = score;

    if (highscore >= beta) {
        return highscore;
    }
    if (highscore > alpha) {
        alpha = highscore;
    }

    size_t move_count = 0;

    struct move* moves = malloc(sizeof(struct move[MOVE_MAX]));
    if (!moves) {
        abort();
    }

    /*struct move moves[MOVE_MAX];*/

    all_moves(pos, us, &move_count, moves);
    if (move_count == 0) {
        /* TODO: detect stalemate */
        free(moves);
        return -(999.0 + (double)depth);
    }
    for (size_t i = 0; i < move_count; ++i) {
        move_compute_appeal(&moves[i], pos, us, mailbox);
    }

    while (move_count) {
        struct move m = moves_linear_search(moves, &move_count);

        if ((m.attr & MATTR_CAPTURE) == 0) {
            continue;
        }

        /* TODO: make lean apply/undo mechanism instead of copying,
         * use of malloc is particularly horrendous */
        struct pos* poscpy = malloc(sizeof *poscpy);
        if (!poscpy) {
            perror("malloc");
            abort();
        }
        *poscpy = *pos;
        enum piece* mailbox_cpy = malloc(sizeof (enum piece[SQ_INDEX_COUNT]));
        if (!mailbox_cpy) {
            perror("malloc");
            abort();
        }
        memcpy(mailbox_cpy, mailbox, sizeof (enum piece[SQ_INDEX_COUNT]));

        /* history is irrelevant when all moves are captures */
        static struct history hist;
        hist.length = 0;
        (void)board_move(poscpy, &hist, mailbox_cpy, m);

        score = -quiesce(poscpy, mailbox_cpy, them, -beta, -alpha, depth - 1);

        free(poscpy);
        free(mailbox_cpy);

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

    free(moves);

    return highscore;
}


struct search_option alphabeta_search(struct pos const* pos,
                                      struct history*   hist,
                                      struct tt*        tt,
                                      enum piece        mailbox[restrict static SQ_INDEX_COUNT],
                                      enum player       us,
                                      int8_t            depth,
                                      double            alpha,
                                      double            beta)
{

    const double alpha_orig = alpha;

    // Terminal / leaf
    if (depth <= 0) {
        return (struct search_option) {
            /*.score  = quiesce(pos, mailbox, us, alpha, beta, depth),*/
            .score  = board_score_heuristic(pos),
            .move   = (struct move){0},
            .depth  = 0,
            .hash   = pos->hash,
            .init   = true,
            .flag   = TT_EXACT,
        };
    }

    /* TT probe at current node */
    global_stats.tt_probes += 1;
    struct search_option tte = tt_get(tt, pos->hash);

#ifndef NDEBUG
    if (tte.init && tte.hash == pos->hash) {
        global_stats.tt_hits += 1;
    } else if (tte.init && tte.hash != pos->hash) {
        global_stats.tt_collisions += 1;
    }
#endif

    if (tte.init && tte.hash == pos->hash && tte.depth >= depth) {
        if (tte.flag == TT_EXACT) {
            return tte;
        } else if (tte.flag == TT_LOWER) {
            if (tte.score > alpha) alpha = tte.score;
        } else if (tte.flag == TT_UPPER) {
            if (tte.score < beta) beta = tte.score;
        }
        if (alpha >= beta) {
            return tte;
        }
    }
    struct move moves[MOVE_MAX];
    size_t move_count = 0ULL;
    all_moves(pos, us, &move_count, moves);

    if (move_count == 0) {
        /* TODO: detect stalemate */

        /* TODO: reusing mate distances correctly needs ply normalization */
        return (struct search_option) {
            .score  = -(999.0 + (double)depth),
            .move   = (struct move){0},
            .depth  = depth,
            .hash   = pos->hash,
            .init   = true,
            .flag   = TT_EXACT,
        };
    }

    for (size_t i = 0; i < move_count; ++i) {
        move_compute_appeal(&moves[i], pos, us, mailbox);
    }

    /* if TT had a best move for this position, search it first. */
    if (tte.init && tte.hash == pos->hash) {
        for (size_t i = 0; i < move_count; ++i) {
            if (moves[i].from == tte.move.from && moves[i].to == tte.move.to) {
                moves[i].appeal = APPEAL_MAX;
                break;
            }
        }
    }

    double best_score = -1e300;
    struct move best_move = moves[0];


    while (move_count) {
        struct move m = moves_linear_search(moves, &move_count);

        /* TODO: make lean apply/undo mechanism instead of copying */
        struct pos poscpy = *pos;
        enum piece mailbox_cpy[SQ_INDEX_COUNT];
        memcpy(mailbox_cpy, mailbox, sizeof mailbox_cpy);
        size_t old_hist_length = hist->length;

        enum move_result const r = board_move(&poscpy, hist, mailbox_cpy, m);

        double score;
        if (r == MR_STALEMATE) {
            score = 0.0;
        } else {
            score = -alphabeta_search(&poscpy,
                                      hist,
                                      tt,
                                      mailbox_cpy,
                                      opposite_player(us),
                                      depth - 1,
                                      -beta,
                                      -alpha).score;
        }

        hist->length = old_hist_length;

        if (score > best_score) {
            best_score = score;
            best_move  = m;
        }

        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            struct search_option out = {
                .score  = alpha,
                .move   = best_move,
                .depth  = depth,
                .hash   = pos->hash,
                .init   = true,
                .flag   = TT_LOWER,
            };
            tt_insert(tt, pos->hash, out);
            return out;
        }
    }

    enum tt_flag flag = TT_EXACT;
    if (best_score <= alpha_orig) flag = TT_UPPER;

    struct search_option out = {
        .score  = best_score,
        .move   = best_move,
        .depth  = depth,
        .hash   = pos->hash,
        .init   = true,
        .flag   = flag,
    };
    tt_insert(tt, pos->hash, out);
    return out;
}

struct move search(struct board* b, enum player us, int8_t max_depth)
{
    if (b->tt.entries == NULL) {
        b->tt.entries = calloc(TT_ENTRIES, sizeof b->tt.entries[0]);
        if (b->tt.entries == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
    }

    struct move best_move = {0};

    double score = 0.0;

#define SCORE_INF 1e80

    for (int8_t d = 1; d <= max_depth; ++d) {
        double window = 0.5; /* half a pawn */
        double alpha = score - window;
        double beta = score + window;

        while (true) {
            struct search_option so =
                alphabeta_search(&b->pos, &b->hist, &b->tt, b->mailbox, us, d, alpha, beta);

            if (so.score > alpha && so.score < beta) {
                score = so.score;
                best_move = so.move;
                break;
            }

            window *= 2;

            alpha = score - window;
            beta = score + window;
            
            if (so.score <= alpha) {
                alpha = -SCORE_INF;
            } else {
                beta = SCORE_INF;
            }

        }
        fprintf(stderr, "depth: %d\n", d);
    }

#undef SCORE_INF

    return best_move;
}


static void board_print(const struct pos* pos, struct move* move, FILE* out)
{
    int buf[8][8] = {0};

    for (enum player player = PLAYER_BEGIN; player < PLAYER_COUNT; ++player) {
        for (enum piece piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
            bitboard x = pos->pieces[player][piece];

            for (index i = 7; i < 8; i--) {
                for (index j = 0; j < 8; ++j) {
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
        for (index j = 0; j < 8; ++j) {
            index const n = INDEX_FROM_RF(i,j);
            if (move && n == move->from) {
                fprintf(out, "\033[%d;%dm", 30, 44); /* 44: blue*/
            } else if (move && n == move->to) {
                fprintf(out, "\033[%d;%dm", 30, 44); /* 104: bright blue*/
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

static void bitboard_print(bitboard b, FILE* out)
{
    setlocale(LC_ALL, "");
    fprintf(out, "\n");
    for (index i = 7; i < 8; i--) {
        fprintf(out, "\033[0m"); /* reset background color */
        fprintf(out, "%"INDEX_FMT" ", i+1);
        for (index j = 0; j < 8; ++j) {
            index const n = INDEX_FROM_RF(i,j);
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

static void board_print_threats(const struct pos* pos, FILE* out, struct move* move)
{
    enum player const player = pos->player;
    int buf[8][8] = {0};

    bitboard const threats = all_threats_from_player(pos, player);

    for (enum player player = PLAYER_BEGIN; player < PLAYER_COUNT; ++player) {
        for (enum piece piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
            bitboard x = pos->pieces[player][piece];

            for (index i = 7; i < 8; i--) {
                for (index j = 0; j < 8; ++j) {
                    if (x & (1ULL<<(i*8+j))) {
                        buf[i][j] = piece_unicode[player][piece];
                    }
                }
            }
        }
    }

    /* see: https://en.wikipedia.org/wiki/ANSI_escape_code#Colors */
    setlocale(LC_ALL, ""); /* needed for unicode symbols */
    for (index i = 7; i < 8; i--) {
        fprintf(out, "\033[0m"); /* reset background color */
        fprintf(out, "%"INDEX_FMT" ", i+1);
        for (index j = 0; j < 8; ++j) {
            index const n = INDEX_FROM_RF(i,j);

            if (move && n == move->from) {
                fprintf(out, "\033[%d;%dm", 30, 44); /* 44: blue*/
            } else if (move && n == move->to) {
                fprintf(out, "\033[%d;%dm", 30, 44); /* 104: bright blue*/
            }
            else if ((threats >> n) & 1) {
                fprintf(out, "\033[%d;%dm", 30, 41); /* 41: red */
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
