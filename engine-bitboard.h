#pragma once

typedef uint64_t Bb64;

#define BITBOARD(                \
        a8,b8,c8,d8,e8,f8,g8,h8, \
        a7,b7,c7,d7,e7,f7,g7,h7, \
        a6,b6,c6,d6,e6,f6,g6,h6, \
        a5,b5,c5,d5,e5,f5,g5,h5, \
        a4,b4,c4,d4,e4,f4,g4,h4, \
        a3,b3,c3,d3,e3,f3,g3,h3, \
        a2,b2,c2,d2,e2,f2,g2,h2, \
        a1,b1,c1,d1,e1,f1,g1,h1) \
(Bb64)\
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

#define FILE_MASK(n) MASK_SHIFT_EAST((Bb64)0x0101010101010101ULL, n)
#define RANK_MASK(n) MASK_SHIFT_NORTH((Bb64)0x00000000000000FFULL, n)
#define MASK_FROM_SQ(idx) (((Bb64)1ULL)<<((Index8)idx))
#define MASK_FROM_RF(rank, file) (MASK_FROM_SQ(SQ_FROM_RF(rank,file)))

enum : Bb64 {
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

    MASK_SHIFT_EAST_GUARD_0 = ~0ULL,
    MASK_SHIFT_EAST_GUARD_1 = ~FILE_MASK_H,
    MASK_SHIFT_EAST_GUARD_2 = ~(FILE_MASK_G | FILE_MASK_H),
    MASK_SHIFT_EAST_GUARD_3 = ~(FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),
    MASK_SHIFT_EAST_GUARD_4 = ~(FILE_MASK_E | FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),
    MASK_SHIFT_EAST_GUARD_5 = ~(FILE_MASK_D | FILE_MASK_E | FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),
    MASK_SHIFT_EAST_GUARD_6 = ~(FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),
    MASK_SHIFT_EAST_GUARD_7 = ~(FILE_MASK_B | FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F | FILE_MASK_G | FILE_MASK_H),

    MASK_SHIFT_WEST_GUARD_0  = ~0ULL,
    MASK_SHIFT_WEST_GUARD_1  = ~FILE_MASK_A,
    MASK_SHIFT_WEST_GUARD_2  = ~(FILE_MASK_A | FILE_MASK_B),
    MASK_SHIFT_WEST_GUARD_3  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C),
    MASK_SHIFT_WEST_GUARD_4  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C | FILE_MASK_D),
    MASK_SHIFT_WEST_GUARD_5  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C | FILE_MASK_D | FILE_MASK_E),
    MASK_SHIFT_WEST_GUARD_6  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F),
    MASK_SHIFT_WEST_GUARD_7  = ~(FILE_MASK_A | FILE_MASK_B | FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F | FILE_MASK_G),
};

#define _MASK_SHIFT_EAST_GUARDED(b, n) \
    (((Bb64)(b) & MASK_SHIFT_EAST_GUARD_##n) << (Index8)(n))

#define _MASK_SHIFT_WEST_GUARDED(b, n)  \
    (((Bb64)(b) & MASK_SHIFT_WEST_GUARD_##n) >> (Index8)(n))

/* MASK_SHIFT_EAST_n shifts the bitboard right by n ranks without wrapping */
#define MASK_SHIFT_EAST_1(b) _MASK_SHIFT_EAST_GUARDED(b, 1)
#define MASK_SHIFT_EAST_2(b) _MASK_SHIFT_EAST_GUARDED(b, 2)
#define MASK_SHIFT_EAST_3(b) _MASK_SHIFT_EAST_GUARDED(b, 3)
#define MASK_SHIFT_EAST_4(b) _MASK_SHIFT_EAST_GUARDED(b, 4)
#define MASK_SHIFT_EAST_5(b) _MASK_SHIFT_EAST_GUARDED(b, 5)
#define MASK_SHIFT_EAST_6(b) _MASK_SHIFT_EAST_GUARDED(b, 6)
#define MASK_SHIFT_EAST_7(b) _MASK_SHIFT_EAST_GUARDED(b, 7)
#define MASK_SHIFT_EAST_8(b) _MASK_SHIFT_EAST_GUARDED(b, 8)

/* MASK_SHIFT_WEST_n shifts the bitboard left by n ranks without wrapping */
#define MASK_SHIFT_WEST_1(b) _MASK_SHIFT_WEST_GUARDED(b, 1)
#define MASK_SHIFT_WEST_2(b) _MASK_SHIFT_WEST_GUARDED(b, 2)
#define MASK_SHIFT_WEST_3(b) _MASK_SHIFT_WEST_GUARDED(b, 3)
#define MASK_SHIFT_WEST_4(b) _MASK_SHIFT_WEST_GUARDED(b, 4)
#define MASK_SHIFT_WEST_5(b) _MASK_SHIFT_WEST_GUARDED(b, 5)
#define MASK_SHIFT_WEST_6(b) _MASK_SHIFT_WEST_GUARDED(b, 6)
#define MASK_SHIFT_WEST_7(b) _MASK_SHIFT_WEST_GUARDED(b, 7)
#define MASK_SHIFT_WEST_8(b) _MASK_SHIFT_WEST_GUARDED(b, 8)

/* MASK_SHIFT_NORTH_n shifts the bitboard towards rank 8 by n ranks without wrapping */
#define MASK_SHIFT_NORTH_1(b) MASK_SHIFT_NORTH(b, 1)
#define MASK_SHIFT_NORTH_2(b) MASK_SHIFT_NORTH(b, 2)
#define MASK_SHIFT_NORTH_3(b) MASK_SHIFT_NORTH(b, 3)
#define MASK_SHIFT_NORTH_4(b) MASK_SHIFT_NORTH(b, 4)
#define MASK_SHIFT_NORTH_5(b) MASK_SHIFT_NORTH(b, 5)
#define MASK_SHIFT_NORTH_6(b) MASK_SHIFT_NORTH(b, 6)
#define MASK_SHIFT_NORTH_7(b) MASK_SHIFT_NORTH(b, 7)
#define MASK_SHIFT_NORTH_8(b) MASK_SHIFT_NORTH(b, 8)

/* MASK_SHIFT_SOUTH_n shifts the bitboard towards rank 1 by n ranks without wrapping */
#define MASK_SHIFT_SOUTH_1(b) MASK_SHIFT_SOUTH(b, 1)
#define MASK_SHIFT_SOUTH_2(b) MASK_SHIFT_SOUTH(b, 2)
#define MASK_SHIFT_SOUTH_3(b) MASK_SHIFT_SOUTH(b, 3)
#define MASK_SHIFT_SOUTH_4(b) MASK_SHIFT_SOUTH(b, 4)
#define MASK_SHIFT_SOUTH_5(b) MASK_SHIFT_SOUTH(b, 5)
#define MASK_SHIFT_SOUTH_6(b) MASK_SHIFT_SOUTH(b, 6)
#define MASK_SHIFT_SOUTH_7(b) MASK_SHIFT_SOUTH(b, 7)
#define MASK_SHIFT_SOUTH_8(b) MASK_SHIFT_SOUTH(b, 8)

enum {
    #define X(file, rank) SQMASK_##file##rank = MASK_FROM_RF(RANK_INDEX_##rank, FILE_INDEX_##file),
    SQUARES_LIST
    #undef X
};

static Sq8 bitboard_lsb(Bb64 p)
{
    assuming(p != 0);
    return (Sq8)__builtin_ffsll((int64_t)p) - 1;
}

static Sq8 bitboard_pop_lsb(Bb64* p)
{
    Sq8 const lsb = bitboard_lsb(*p);
    *p &= ~(1ULL<<(lsb));
    return lsb;
}

static inline uint64_t bitboard_popcount(Bb64 b)
{
    return (uint64_t)__builtin_popcountll(b);
}

static inline bool bitboard_more_than_one(Bb64 b)
{
    return b & (b - 1);
}

