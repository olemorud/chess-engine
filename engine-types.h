#pragma once

/* ----------- Index8 ----------- */
typedef uint8_t Index8;

#define MASK_SHIFT_NORTH(b, n) ((b) << ((Index8)(n)*8U))
#define MASK_SHIFT_SOUTH(b, n) ((b) >> ((Index8)(n)*8U))
#define MASK_SHIFT_WEST(b, n)  ((b) >> ((Index8)(n)*1U))
#define MASK_SHIFT_EAST(b, n) ((b) << ((Index8)(n)*1U))

#define SQ_SHIFT_NORTH(i, n)    ((i) + ((Index8)(n)*8U))
#define SQ_SHIFT_SOUTH(i, n)  ((i) - ((Index8)(n)*8U))
#define SQ_SHIFT_WEST(i, n)  ((i) - ((Index8)(n)*1U))
#define SQ_SHIFT_EAST(i, n) ((i) + ((Index8)(n)*1U))

#define SQ_FROM_RF(rank, file) ((Index8)(8U*(Index8)(rank) + (Index8)(file)))

/* ----------- file_index ----------- */
enum file_index : Index8 {
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

char const file_index_char[FILE_INDEX_COUNT] = {
    [FILE_INDEX_A] = 'A',
    [FILE_INDEX_B] = 'B',
    [FILE_INDEX_C] = 'C',
    [FILE_INDEX_D] = 'D',
    [FILE_INDEX_E] = 'E',
    [FILE_INDEX_F] = 'F',
    [FILE_INDEX_G] = 'G',
    [FILE_INDEX_H] = 'H',
};

/* ----------- rank_index ----------- */
enum rank_index : Index8 {
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

char const rank_index_char[RANK_INDEX_COUNT] = {
    [RANK_INDEX_1] = '1',
    [RANK_INDEX_2] = '2',
    [RANK_INDEX_3] = '3',
    [RANK_INDEX_4] = '4',
    [RANK_INDEX_5] = '5',
    [RANK_INDEX_6] = '6',
    [RANK_INDEX_7] = '7',
    [RANK_INDEX_8] = '8',
};

/* ----------- Sq8 ----------- */

#define SQMASK_PREFIX SQMASK_
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

typedef enum sq8 : uint8_t {
    #define X(file, rank) SQ_##file##rank = SQ_FROM_RF(RANK_INDEX_##rank, FILE_INDEX_##file),
    SQUARES_LIST
    SQ_COUNT,
    #undef X
    /* define iterator begin enum */
    #define X(file, rank) SQ_BEGIN = SQ_##file##rank,
    SQUARES_LIST_BEGIN
    #undef X
} Sq8;

static char* const sq8_display[SQ_COUNT] = {
    #define X(file, rank) \
    [SQ_##file##rank] = STR(file##rank),
    SQUARES_LIST
    #undef X
};

static char* const sq8_str[SQ_COUNT] = {
    #define X(file, rank) \
    [SQ_##file##rank] = STR(SQ_##file##rank),
    SQUARES_LIST
    #undef X
};

static inline enum file_index sq_to_file(Sq8 p)
{
    return p % 8ULL;
}

static inline enum rank_index sq_to_rank(Sq8 p)
{
    return p / 8ULL;
}

/* ----------- Side8 ----------- */
typedef enum side : uint8_t {
    SIDE_WHITE,
    SIDE_BLACK,
    SIDE_COUNT,
    SIDE_BEGIN = SIDE_WHITE,
} Side8;

static inline Side8 other_side(Side8 p)
{
    return (p == SIDE_WHITE ? SIDE_BLACK : SIDE_WHITE);
}

static char const* side_str[SIDE_COUNT] = {
    [SIDE_WHITE] = "SIDE_WHITE",
    [SIDE_BLACK] = "SIDE_BLACK",
};

/* ----------- Piece8 ----------- */

/* https://en.wikipedia.org/wiki/X_macro */
/*    enum          value   white char   white unicode   black char    black unicode  */
#define PIECES \
    X(PIECE_PAWN,     1.0,         'P',         0x2659,         'p',          0x265F) \
    X(PIECE_KING,     0.0,         'K',         0x2654,         'k',          0x265A) \
    X(PIECE_QUEEN,    9.0,         'Q',         0x2655,         'q',          0x265B) \
    X(PIECE_BISHOP,   3.0,         'B',         0x2657,         'b',          0x265D) \
    X(PIECE_ROOK,     5.0,         'R',         0x2656,         'r',          0x265C) \
    X(PIECE_KNIGHT,   3.0,         'N',         0x2658,         'n',          0x265E)

typedef enum piece : uint8_t {
#define X(e, v, wc, wu, bc, bu) e,
    PIECES
    PIECE_COUNT,
    PIECE_BEGIN = 0,
#undef X
} Piece8;

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

struct piece_side {Piece8 piece; Side8 side;} const piece_and_side_from_char[256] = {
    #define X(e, v, wc, wu, bc, bu) \
        [(uint8_t)bc] = {.piece = e, .side = SIDE_BLACK}, \
        [(uint8_t)wc] = {.piece = e, .side = SIDE_WHITE},
    PIECES
    #undef X
};

static char const piece_char[SIDE_COUNT][PIECE_COUNT] = {
    [SIDE_WHITE] = {
#define X(e, v, wc, wu, bc, bu) [e] = wc,
        PIECES
#undef X
    },
    [SIDE_BLACK] = {
#define X(e, v, wc, wu, bc, bu) [e] = bc,
        PIECES
#undef X
    }
};

static int const piece_unicode[SIDE_COUNT][PIECE_COUNT] = {
    [SIDE_WHITE] = {
#define X(e, v, wc, wu, bc, bu) [e] = wu,
        PIECES
#undef X
    },
    [SIDE_BLACK] = {
#define X(e, v, wc, wu, bc, bu) [e] = bu,
        PIECES
#undef X
    }
};


/* ----------- moves ----------- */

enum {
    MATTR_PROMOTE = 1<<0,
};

struct move {
    Sq8 from;
    Sq8 to;
    uint8_t attr;
    #define APPEAL_MAX UINT8_MAX
    uint8_t  appeal;
};
_Static_assert(sizeof(struct move) == 4,
        "this static assuming is here to check when sizeof(move) changes");

#define NULL_MOVE (struct move){0}

#define IS_NULL_MOVE(m) ((m).from == (m).to)

/* ----------- castle_direction ----------- */
enum castle_direction {
    CASTLE_BEGIN,
    CASTLE_KINGSIDE = CASTLE_BEGIN,
    CASTLE_QUEENSIDE,
    CASTLE_COUNT,
};
