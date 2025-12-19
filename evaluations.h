/* TODO: candidate for code-generation */

#pragma once

#define BITBOARD_WHITE(          \
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

#define BITBOARD_BLACK(          \
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
h1##g1##f1##e1##d1##c1##b1##a1##\
h2##g2##f2##e2##d2##c2##b2##a2##\
h3##g3##f3##e3##d3##c3##b3##a3##\
h4##g4##f4##e4##d4##c4##b4##a4##\
h5##g5##f5##e5##d5##c5##b5##a5##\
h6##g6##f6##e6##d6##c6##b6##a6##\
h7##g7##f7##e7##d7##c7##b7##a7##\
h8##g8##f8##e8##d8##c8##b8##a8##\
ULL

#define RELATIVE_DIAGONAL_A1_H8 \
    RELATIVE_BITBOARD( \
        0,0,0,0,0,0,0,1, \
        0,0,0,0,0,0,1,0, \
        0,0,0,0,0,1,0,0, \
        0,0,0,0,1,0,0,0, \
        0,0,0,1,0,0,0,0, \
        0,0,1,0,0,0,0,0, \
        0,1,0,0,0,0,0,0, \
        1,0,0,0,0,0,0,0)

#define RELATIVE_DIAGONAL_A8_H1 \
    RELATIVE_BITBOARD( \
        1,0,0,0,0,0,0,0, \
        0,1,0,0,0,0,0,0, \
        0,0,1,0,0,0,0,0, \
        0,0,0,1,0,0,0,0, \
        0,0,0,0,1,0,0,0, \
        0,0,0,0,0,1,0,0, \
        0,0,0,0,0,0,1,0, \
        0,0,0,0,0,0,0,1)

#define RELATIVE_BISHOP_KING_ATTACK \
    RELATIVE_BITBOARD( \
        0,0,0,0,0,0,1,1, \
        0,0,0,0,0,1,1,0, \
        0,0,0,0,1,1,0,0, \
        0,0,0,1,1,0,0,0, \
        0,0,1,1,0,0,0,0, \
        0,1,1,0,0,0,0,0, \
        1,1,0,0,0,0,0,0, \
        1,0,0,0,0,0,0,0)

#define RELATIVE_BISHOP_QUEEN_ATTACK \
    RELATIVE_BITBOARD( \
        1,1,0,0,0,0,0,0, \
        0,1,1,0,0,0,0,0, \
        0,0,1,1,0,0,0,0, \
        0,0,0,1,1,0,0,0, \
        0,0,0,0,1,1,0,0, \
        0,0,0,0,0,1,1,0, \
        0,0,0,0,0,0,1,1, \
        0,0,0,0,0,0,0,1)

#define RELATIVE_KING_CASTLE_KINGSIDE \
    RELATIVE_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,1,0)

#define RELATIVE_KING_CASTLE_QUEENSIDE \
    RELATIVE_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,1,1,0,0,0,0,0)

#define CORNERS \
    BITBOARD( \
        1,0,0,0,0,0,0,1, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        1,0,0,0,0,0,0,1)

#define RELATIVE_PAWN_SAFE_ZONE \
    BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        1,0,0,0,0,0,0,0, \
        1,0,0,1,1,0,0,0, \
        1,1,1,1,1,0,0,0, \
        1,1,1,1,1,0,0,1, \
        1,1,1,1,1,1,1,1, \
        0,0,0,0,0,0,0,0)

#define BOARD_CENTER_4X4 \
    ((FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F) \
    & (RANK_MASK_3 | RANK_MASK_4 | RANK_MASK_5 | RANK_MASK_6))

#define BOARD_CENTER_2X2 \
    ((FILE_MASK_D | FILE_MASK_E) \
    & (RANK_MASK_4 | RANK_MASK_5))


#define POSITIONAL_BONUS_0 \
   /* piece         bonus   area*/ \
    X(PIECE_PAWN,    0.02,  BOARD_CENTER_4X4) \
    X(PIECE_KNIGHT,  0.05,  BOARD_CENTER_4X4) \
    X(PIECE_BISHOP,  0.05,  RELATIVE_DIAGONAL_A1_H8 | RELATIVE_DIAGONAL_A8_H1) \
    X(PIECE_KING,    0.15,  RELATIVE_KING_CASTLE_KINGSIDE) \
    X(PIECE_QUEEN,  -0.10,  RANK_MASK_3 | RANK_MASK_4 | RANK_MASK_5 | RANK_MASK_6) \
    /**/

#define POSITIONAL_BONUS_1 \
   /* piece         bonus   area*/ \
    X(PIECE_PAWN,   0.02,   BOARD_CENTER_2X2) \
    X(PIECE_BISHOP, 0.05,   RELATIVE_BISHOP_KING_ATTACK) \
    /**/

#define POSITIONAL_BONUS_2 \
   /* piece         bonus   area*/ \
    X(PIECE_PAWN,   -0.10,  ~RELATIVE_PAWN_SAFE_ZONE) \
    X(PIECE_BISHOP,  0.05,   CORNERS) \
    /**/

#define POSITIONAL_BONUS_3 \
   /* piece         bonus   area*/ \
    X(PIECE_BISHOP, 0.02,   RELATIVE_BISHOP_QUEEN_ATTACK)

#define POSITIONAL_MODIFIER_COUNT 4
static struct {bitboard const area; double const val;} const
positional_modifier[PLAYER_COUNT][POSITIONAL_MODIFIER_COUNT][PIECE_COUNT] = {
#define X(p, b, a) [p] = {.area = (a), .val = b},
#define RELATIVE_BITBOARD BITBOARD_WHITE
    [PLAYER_WHITE] = {
            {POSITIONAL_BONUS_0},
            {POSITIONAL_BONUS_1},
            {POSITIONAL_BONUS_2},
            {POSITIONAL_BONUS_3},
    },
    [PLAYER_BLACK] = {
#undef RELATIVE_BITBOARD
#define RELATIVE_BITBOARD BITBOARD_BLACK
            {POSITIONAL_BONUS_0},
            {POSITIONAL_BONUS_1},
            {POSITIONAL_BONUS_2},
            {POSITIONAL_BONUS_3},
    }
#undef X
};


#undef CORNERS
#undef BOARD_CENTER_4X4
#undef BOARD_CENTER_2X2
#undef BITBOARD_WHITE
#undef BITBOARD_BLACK
#undef RELATIVE_DIAGONAL_A1_H8 
#undef RELATIVE_DIAGONAL_A8_H1 
#undef RELATIVE_BISHOP_KING_ATTACK 
#undef RELATIVE_BISHOP_QUEEN_ATTACK 
#undef RELATIVE_KING_CASTLE_KINGSIDE 
#undef RELATIVE_KING_CASTLE_QUEENSIDE 
