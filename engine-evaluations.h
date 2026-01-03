/* TODO: candidate for code-generation */

#pragma once

enum game_progress {
    GP_OPENING,
    GP_MIDDLE,
    GP_END,
    GP_COUNT,
};

static char const* game_progress_str[GP_COUNT] = {
    [GP_OPENING] = "GP_OPENING",
    [GP_MIDDLE]  = "GP_MIDDLE",
    [GP_END]     = "GP_END",
};

static enum game_progress endgameness(struct pos const* pos)
{
    /* piece_value is already defined similarly elsewhere, but this one should
     * remain independent of minor tweaks in the global table */
    static int const piece_value[PIECE_COUNT] = {
        [PIECE_KING] = 0,
        [PIECE_PAWN] = 1,
        [PIECE_BISHOP] = 3,
        [PIECE_KNIGHT] = 3,
        [PIECE_ROOK] = 5,
        [PIECE_QUEEN] = 9,
    };

    int npm = 0; /* non-pawn material */
    for (Side8 pl = SIDE_BEGIN; pl < SIDE_COUNT; ++pl) {
        npm += piece_value[PIECE_QUEEN]  * bitboard_popcount(pos->pieces[pl][PIECE_QUEEN]);
        npm += piece_value[PIECE_BISHOP] * bitboard_popcount(pos->pieces[pl][PIECE_BISHOP]);
        npm += piece_value[PIECE_KNIGHT] * bitboard_popcount(pos->pieces[pl][PIECE_KNIGHT]);
        npm += piece_value[PIECE_ROOK]   * bitboard_popcount(pos->pieces[pl][PIECE_ROOK]);
    }

    /**/ if (npm > 24) {
        return GP_OPENING;
    }
    else if (npm > 16) {
        return GP_MIDDLE;
    }
    else {
        return GP_END;
    }
}

#define BITBOARD_WHITE(          \
        a8,b8,c8,d8,e8,f8,g8,h8, \
        a7,b7,c7,d7,e7,f7,g7,h7, \
        a6,b6,c6,d6,e6,f6,g6,h6, \
        a5,b5,c5,d5,e5,f5,g5,h5, \
        a4,b4,c4,d4,e4,f4,g4,h4, \
        a3,b3,c3,d3,e3,f3,g3,h3, \
        a2,b2,c2,d2,e2,f2,g2,h2, \
        a1,b1,c1,d1,e1,f1,g1,h1) \
BITBOARD( \
        a8,b8,c8,d8,e8,f8,g8,h8, \
        a7,b7,c7,d7,e7,f7,g7,h7, \
        a6,b6,c6,d6,e6,f6,g6,h6, \
        a5,b5,c5,d5,e5,f5,g5,h5, \
        a4,b4,c4,d4,e4,f4,g4,h4, \
        a3,b3,c3,d3,e3,f3,g3,h3, \
        a2,b2,c2,d2,e2,f2,g2,h2, \
        a1,b1,c1,d1,e1,f1,g1,h1)

#define BITBOARD_BLACK(          \
        a8,b8,c8,d8,e8,f8,g8,h8, \
        a7,b7,c7,d7,e7,f7,g7,h7, \
        a6,b6,c6,d6,e6,f6,g6,h6, \
        a5,b5,c5,d5,e5,f5,g5,h5, \
        a4,b4,c4,d4,e4,f4,g4,h4, \
        a3,b3,c3,d3,e3,f3,g3,h3, \
        a2,b2,c2,d2,e2,f2,g2,h2, \
        a1,b1,c1,d1,e1,f1,g1,h1) \
BITBOARD( \
        a1,b1,c1,d1,e1,f1,g1,h1, \
        a2,b2,c2,d2,e2,f2,g2,h2, \
        a3,b3,c3,d3,e3,f3,g3,h3, \
        a4,b4,c4,d4,e4,f4,g4,h4, \
        a5,b5,c5,d5,e5,f5,g5,h5, \
        a6,b6,c6,d6,e6,f6,g6,h6, \
        a7,b7,c7,d7,e7,f7,g7,h7, \
        a8,b8,c8,d8,e8,f8,g8,h8)

#define REL_RANK_1 \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        1,1,1,1,1,1,1,1)

#define REL_RANK_2 \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        1,1,1,1,1,1,1,1, \
        0,0,0,0,0,0,0,0)

#define REL_RANK_3 \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        1,1,1,1,1,1,1,1, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0)

#define REL_RANK_4 \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        1,1,1,1,1,1,1,1, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0)

#define REL_RANK_5 \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        1,1,1,1,1,1,1,1, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0)

#define REL_RANK_6 \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        1,1,1,1,1,1,1,1, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0)

#define REL_RANK_7 \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        1,1,1,1,1,1,1,1, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0)

#define REL_RANK_8 \
    REL_BITBOARD( \
        1,1,1,1,1,1,1,1, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0)


#define DIAGONAL_A1_H8 \
    BITBOARD( \
        0,0,0,0,0,0,0,1, \
        0,0,0,0,0,0,1,0, \
        0,0,0,0,0,1,0,0, \
        0,0,0,0,1,0,0,0, \
        0,0,0,1,0,0,0,0, \
        0,0,1,0,0,0,0,0, \
        0,1,0,0,0,0,0,0, \
        1,0,0,0,0,0,0,0)

#define DIAGONAL_A8_H1 \
    BITBOARD( \
        1,0,0,0,0,0,0,0, \
        0,1,0,0,0,0,0,0, \
        0,0,1,0,0,0,0,0, \
        0,0,0,1,0,0,0,0, \
        0,0,0,0,1,0,0,0, \
        0,0,0,0,0,1,0,0, \
        0,0,0,0,0,0,1,0, \
        0,0,0,0,0,0,0,1)

#define REL_BISHOP_KING_ATTACK \
    REL_BITBOARD( \
        0,0,0,0,0,0,1,1, \
        0,0,0,0,0,1,1,0, \
        0,0,0,0,1,1,0,0, \
        0,0,0,1,1,0,0,0, \
        0,0,1,1,0,0,0,0, \
        0,1,1,0,0,0,0,0, \
        1,1,0,0,0,0,0,0, \
        1,0,0,0,0,0,0,0)

#define REL_BISHOP_QUEEN_ATTACK \
    REL_BITBOARD( \
        1,1,0,0,0,0,0,0, \
        0,1,1,0,0,0,0,0, \
        0,0,1,1,0,0,0,0, \
        0,0,0,1,1,0,0,0, \
        0,0,0,0,1,1,0,0, \
        0,0,0,0,0,1,1,0, \
        0,0,0,0,0,0,1,1, \
        0,0,0,0,0,0,0,1)

#define REL_KING_CASTLE_KINGSIDE \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,1,0)

#define REL_KING_CASTLE_QUEENSIDE \
    REL_BITBOARD( \
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

#define REL_EARLY_PAWN_STRUCTURE \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        1,0,0,0,0,0,0,0, \
        1,1,1,1,1,0,0,0, \
        1,1,1,1,1,0,0,0, \
        1,1,1,1,1,0,0,0, \
        1,1,1,1,1,1,1,1, \
        0,0,0,0,0,0,0,0)

#define REL_PAWN_KINGSIDE \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,1, \
        0,0,0,0,0,1,1,0, \
        0,0,0,0,0,0,0,0)


#define REL_UNDEVELOPED_BISHOPS \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,1,0,0,1,0,0)

#define REL_UNDEVELOPED_KNIGHTS \
    REL_BITBOARD( \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,1,0,0,0,0,1,0)

#define BOARD_CENTER_6X6 \
    ((FILE_MASK_B | FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F | FILE_MASK_G) \
    & (RANK_MASK_2 | RANK_MASK_3 | RANK_MASK_4 | RANK_MASK_5 | RANK_MASK_6 | RANK_MASK_7))

#define BOARD_CENTER_4X4 \
    ((FILE_MASK_C | FILE_MASK_D | FILE_MASK_E | FILE_MASK_F) \
    & (RANK_MASK_3 | RANK_MASK_4 | RANK_MASK_5 | RANK_MASK_6))

#define BOARD_CENTER_2X2 \
    ((FILE_MASK_D | FILE_MASK_E) \
    & (RANK_MASK_4 | RANK_MASK_5))

#define POSITIONAL_MODIFIER_COUNT 4

/* ------------------------------ early game ------------------------------- */

#define EARLY_POSITIONAL_BONUS_0 \
   /* piece         bonus   area*/ \
    X(PIECE_PAWN,    0.02,  BOARD_CENTER_4X4) \
    X(PIECE_KNIGHT,  0.05,  BOARD_CENTER_4X4) \
    X(PIECE_BISHOP,  0.05,  BOARD_CENTER_6X6 & (DIAGONAL_A1_H8 | DIAGONAL_A8_H1)) \
    X(PIECE_KING,    0.15,  REL_KING_CASTLE_KINGSIDE) \
    X(PIECE_QUEEN,  -0.15,  RANK_MASK_3 | RANK_MASK_4 | RANK_MASK_5 | RANK_MASK_6) \
    X(PIECE_ROOK,    0.10,  FILE_MASK_D | FILE_MASK_E) \
    /**/

#define EARLY_POSITIONAL_BONUS_1 \
   /* piece         bonus   area*/ \
    X(PIECE_PAWN,   0.02,   BOARD_CENTER_2X2) \
    X(PIECE_BISHOP, 0.05,   REL_BISHOP_KING_ATTACK) \
    /**/

#define EARLY_POSITIONAL_BONUS_2 \
   /* piece          bonus   area*/ \
    X(PIECE_PAWN,   -0.18,  ~REL_EARLY_PAWN_STRUCTURE) \
    X(PIECE_KNIGHT, -0.10,   REL_UNDEVELOPED_KNIGHTS) \
    X(PIECE_BISHOP, -0.10,   REL_UNDEVELOPED_BISHOPS) \
    /**/

#define EARLY_POSITIONAL_BONUS_3 \
   /* piece         bonus   area*/ \
    /**/


/* ------------------------------ middle game ------------------------------ */
/* (almost same as opening for now, but queen is not punished for moving) */

#define MIDDLE_POSITIONAL_BONUS_0 \
   /* piece         bonus   area*/ \
    X(PIECE_PAWN,    0.02,  BOARD_CENTER_4X4) \
    X(PIECE_KNIGHT,  0.05,  BOARD_CENTER_4X4) \
    X(PIECE_BISHOP,  0.05,  BOARD_CENTER_6X6 & (DIAGONAL_A1_H8 | DIAGONAL_A8_H1)) \
    X(PIECE_KING,    0.15,  REL_KING_CASTLE_KINGSIDE) \
    /**/

#define MIDDLE_POSITIONAL_BONUS_1 \
   /* piece         bonus   area*/ \
    X(PIECE_PAWN,   0.02,   BOARD_CENTER_2X2) \
    X(PIECE_BISHOP, 0.07,   REL_BISHOP_KING_ATTACK) \
    X(PIECE_QUEEN,  0.07,   REL_BISHOP_KING_ATTACK) \
    /**/

#define MIDDLE_POSITIONAL_BONUS_2 \
   /* piece         bonus   area*/ \
    X(PIECE_PAWN,   0.02,   REL_PAWN_KINGSIDE) \
    /**/

#define MIDDLE_POSITIONAL_BONUS_3 \
   /* piece         bonus   area*/ \
    X(PIECE_BISHOP, 0.05,   BOARD_CENTER_6X6) \
    X(PIECE_KNIGHT, 0.05,   BOARD_CENTER_6X6) \
    /**/

/* ------------------------------- end game -------------------------------- */

#define LATE_POSITIONAL_BONUS_0 \
   /* piece         bonus   area*/ \
    X(PIECE_PAWN,    0.30,  REL_RANK_7 | REL_RANK_6 | REL_RANK_5) \
    X(PIECE_KING,    0.10,  BOARD_CENTER_6X6) \
    /**/

#define LATE_POSITIONAL_BONUS_1 \
   /* piece         bonus    area*/ \
    X(PIECE_PAWN,    0.30,   REL_RANK_7 | REL_RANK_6) \
    X(PIECE_KING,    0.10,   BOARD_CENTER_4X4) \
    /**/

#define LATE_POSITIONAL_BONUS_2 \
   /* piece         bonus    area*/ \
    X(PIECE_PAWN,    0.70,   REL_RANK_7) \
    X(PIECE_KING,    0.10,   BOARD_CENTER_2X2) \
    /**/

#define LATE_POSITIONAL_BONUS_3 \
   /* piece         bonus   area*/ \
    X(PIECE_KING,   -0.50,  ~BOARD_CENTER_6X6) \
    /**/

struct posmod {
    Bb64 const area;
    double const val;
};

static inline struct posmod positional_modifier(Side8 pl, enum game_progress st, size_t layer, Piece8 pz)
{
    static struct posmod const
        lookup[SIDE_COUNT][GP_COUNT][POSITIONAL_MODIFIER_COUNT][PIECE_COUNT] = {
    #define X(p, b, a) [p] = {.area = (a), .val = b},
    #define REL_BITBOARD BITBOARD_WHITE
        [SIDE_WHITE] = {
            [GP_OPENING] = {
                {EARLY_POSITIONAL_BONUS_0},
                {EARLY_POSITIONAL_BONUS_1},
                {EARLY_POSITIONAL_BONUS_2},
                {EARLY_POSITIONAL_BONUS_3},
            },
            [GP_MIDDLE] = {
                {MIDDLE_POSITIONAL_BONUS_0},
                {MIDDLE_POSITIONAL_BONUS_1},
                {MIDDLE_POSITIONAL_BONUS_2},
                {MIDDLE_POSITIONAL_BONUS_3},
            },
            [GP_END] = {
                {LATE_POSITIONAL_BONUS_0},
                {LATE_POSITIONAL_BONUS_1},
                {LATE_POSITIONAL_BONUS_2},
                {LATE_POSITIONAL_BONUS_3},
            },
        },
    #undef REL_BITBOARD

    #define REL_BITBOARD BITBOARD_BLACK
        [SIDE_BLACK] = {
            [GP_OPENING] = {
                {EARLY_POSITIONAL_BONUS_0},
                {EARLY_POSITIONAL_BONUS_1},
                {EARLY_POSITIONAL_BONUS_2},
                {EARLY_POSITIONAL_BONUS_3},
            },
            [GP_MIDDLE] = {
                {MIDDLE_POSITIONAL_BONUS_0},
                {MIDDLE_POSITIONAL_BONUS_1},
                {MIDDLE_POSITIONAL_BONUS_2},
                {MIDDLE_POSITIONAL_BONUS_3},
            },
            [GP_END] = {
                {LATE_POSITIONAL_BONUS_0},
                {LATE_POSITIONAL_BONUS_1},
                {LATE_POSITIONAL_BONUS_2},
                {LATE_POSITIONAL_BONUS_3},
            },
        }
    #undef REL_BITBOARD
    };

    return lookup[pl][st][layer][pz];
}

/* ------------------------------------------------------------------------- */

#undef POSITIONAL_BONUS_0
#undef POSITIONAL_BONUS_1
#undef POSITIONAL_BONUS_2
#undef POSITIONAL_BONUS_3
#undef CORNERS
#undef BOARD_CENTER_4X4
#undef BOARD_CENTER_2X2
#undef BITBOARD_WHITE
#undef BITBOARD_BLACK
#undef REL_DIAGONAL_A1_H8
#undef REL_DIAGONAL_A8_H1
#undef REL_BISHOP_KING_ATTACK
#undef REL_BISHOP_QUEEN_ATTACK
#undef REL_KING_CASTLE_KINGSIDE
#undef REL_KING_CASTLE_QUEENSIDE
