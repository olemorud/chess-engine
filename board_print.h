#pragma once

#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <inttypes.h>

#define BITBOARD_FMT PRIu64
#define BITBOARD_FMT_X PRIx64

#define INDEX_FMT PRIu8

static void bitboard_print(Bb64 b, FILE* out)
{
    setlocale(LC_ALL, "");
    fprintf(out, "\n");
    for (Index8 i = 7; i < 8; i--) {
        fprintf(out, "\033[0m"); /* reset background color */
        fprintf(out, "%"INDEX_FMT" ", i+1);
        for (Index8 j = 0; j < 8; ++j) {
            Index8 const n = SQ_FROM_RF(i,j);
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

static void tt_print_stats(struct tt* tt, FILE* out)
{
#ifndef NDEBUG
    double sat = 0;
    for (size_t i = 0; i < TT_ENTRIES; ++i) {
        if (tt->entries[i].init)
            sat += 1.0;
    }
    double const pct = sat/TT_ENTRIES;

    fprintf(out, "---- Stats ---\n");
    fprintf(out, "tt collisions: %"PRIu64"\n", tt->collisions);
    fprintf(out, "tt hits:       %"PRIu64"\n", tt->hits);
    fprintf(out, "tt probes:     %"PRIu64"\n", tt->probes);
    fprintf(out, "tt insertions: %"PRIu64"\n", tt->insertions);
    fprintf(out, "saturation:    %.02lf\n", pct);
#else
    (void)tt;
    fprintf(out, "stats not available with NDEBUG\n");
#endif
}

static void board_print_fen(struct pos const* pos, FILE* out)
{
    int buf[8][8] = {0};

    for (Side8 side = SIDE_BEGIN; side < SIDE_COUNT; ++side) {
        for (Piece8 piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
                Bb64 x = pos->pieces[side][piece];

            for (Index8 i = 7; i < 8; i--) {
                for (Index8 j = 0; j < 8; ++j) {
                    if (x & (1ULL<<(i*8+j))) {
                        buf[i][j] = piece_char[side][piece];
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

    fprintf(out, " %c ", pos->moving_side == SIDE_WHITE ? 'w' : 'b');

    bool any_castle = false;
    if (!pos->castling_illegal[SIDE_WHITE][CASTLE_KINGSIDE]) {
        fprintf(out, "K");
        any_castle = true;
    }
    if (!pos->castling_illegal[SIDE_WHITE][CASTLE_QUEENSIDE]) {
        fprintf(out, "Q");
        any_castle = true;
    }
    if (!pos->castling_illegal[SIDE_BLACK][CASTLE_KINGSIDE]) {
        fprintf(out, "k");
        any_castle = true;
    }
    if (!pos->castling_illegal[SIDE_BLACK][CASTLE_QUEENSIDE]) {
        fprintf(out, "q");
        any_castle = true;
    }
    if (!any_castle) {
        fprintf(out, "-");
    }

    if (pos->ep_targets) {
        /* should be ep target square in algebraic notation */
        Sq8 const sqi = bitboard_lsb(pos->ep_targets);

        assuming(sqi >= SQ_BEGIN && sqi < SQ_COUNT);

        enum file_index const fi = sq_to_file(sqi);
        enum rank_index const ri = sq_to_rank(sqi);

        int const fch = tolower(file_index_char[fi]);
        int const rch = tolower(rank_index_char[ri]);

        assuming(fch >= 'a' && fch <= 'h');
        assuming(rch >= '1' && rch <= '8');

        fprintf(out, " %c%c", fch, rch);
    } else {
        fprintf(out, " -");
    }

    fprintf(out, " %d", pos->halfmoves);
    fprintf(out, " %d", pos->fullmoves);

    fprintf(out, "\n");
}

static void board_print(struct pos const* pos, struct move* move, FILE* out, bool print_threats)
{
    int buf[8][8] = {0};
    int color[8][8] = {0};

    for (Side8 side = SIDE_BEGIN; side < SIDE_COUNT; ++side) {
        for (Piece8 piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
            Bb64 x = pos->pieces[side][piece];

            for (Index8 i = 7; i < 8; i--) {
                for (Index8 j = 0; j < 8; ++j) {
                    if (x & (1ULL<<(i*8+j))) {
                        buf[i][j] = piece_unicode[SIDE_BLACK][piece];
                        color[i][j] = (side == SIDE_WHITE)
                            ? 1
                            : 2;
                    }
                }
            }
        }
    }

    Bb64 const threats = all_threats_from_side(pos, pos->moving_side);

    /* see: https://en.wikipedia.org/wiki/ANSI_escape_code#Colors */
    setlocale(LC_ALL, "");
    for (Index8 i = 7; i < 8; i--) {
        fprintf(out, "\033[0m"); /* reset background color */
        fprintf(out, "%"INDEX_FMT" ", i+1);
        for (Index8 j = 0; j < 8; ++j) {
            Index8 const n = SQ_FROM_RF(i,j);

            int bg, fg;

            /* foreground color */
            /**/ if (color[i][j] == 1) {
                fg = 97; /* bright white */
            }
            else if (color[i][j] == 2) {
                fg = 30; /* black */
            }
            else {
                fg = 35; /* magenta (should not happen) */
            }

            /* background color */
            /**/ if (move && (n == move->to || n == move->from)) {
                bg = 44; /* bright blue */
            }
            else if (print_threats && (threats >> n) & 1) {
                bg = 41;
            } else {
                /* 45: magenta,
                 * 43: yellow */
                bg = (i+j) % 2 ? 45 : 43;
            }

            if ((pos->occupied[SIDE_WHITE] | pos->occupied[SIDE_BLACK]) & MASK_FROM_SQ(n)) {
                bg += 60; /* make bright */
            }

            fprintf(out, "\033[%d;%dm", fg, bg);

            if (buf[i][j]) {
                fprintf(out, "%lc ", buf[i][j]);
            } else {
                fprintf(out, "  "); /* idk why this hack is needed but "%lc "
                                       is not sufficient when buf[i][j] = ' ' */
            }
        }
        fprintf(out, "\033[0m"); /* reset background color */
        fprintf(out, "\n");
    }
    fprintf(out, "  A B C D E F G H \n");
    fprintf(out, "half moves: %d\n"
                 "full moves: %d\n",
                 pos->halfmoves,
                 pos->fullmoves);
}

