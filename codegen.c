#define CODEGEN
#include "engine.h"
#include "libc-lite.h"
#include "board_print.h"
#include <stdlib.h>
#include <stdio.h>

int main()
{
    struct magic rook_magics[SQ_COUNT] = {0ULL};
    Bb64 rook_attacks[SQ_COUNT][1<<12ULL] = {0};

    Index8 rook_relevant_bits[12] = {0};
    size_t rook_relevant_bits_count = 0;

    struct magic bishop_magics[SQ_COUNT] = {0ULL};
    Bb64 bishop_attacks[SQ_COUNT][1<<9ULL] = {0};

    Index8 bishop_relevant_bits[9] = {0};
    size_t bishop_relevant_bits_count = 0;

    Bb64 between_lookup[SQ_COUNT][SQ_COUNT];

    Bb64 diagonals[SQ_COUNT];

    for (Sq8 sq = SQ_BEGIN; sq < SQ_COUNT; ++sq) {
        diagonals[sq] = diagonals_from_index(sq);
    }

    for (Sq8 sq_index = SQ_BEGIN; sq_index < SQ_COUNT; ++sq_index) {
        enum file_index file = sq_to_file(sq_index);
        enum rank_index rank = sq_to_rank(sq_index);
        fprintf(stderr, "%s ", sq8_display[sq_index]);

        /* ROOKS
         * ======================= */
        {
            /* generate attack mask */
            Bb64 atk_mask = 0;
            {
                atk_mask |= FILE_MASK(file) & ~(RANK_MASK_1 | RANK_MASK_8);
                atk_mask |= RANK_MASK(rank) & ~(FILE_MASK_A | FILE_MASK_H);
                atk_mask &= ~MASK_FROM_RF(rank, file);

                /* populate relevant bits array */
                rook_relevant_bits_count = 0;
                Bb64 x = atk_mask;
                while (x) {
                    Sq8 const lsb = bitboard_pop_lsb(&x);
                    rook_relevant_bits[rook_relevant_bits_count++] = lsb;
                }
            }
            rook_magics[sq_index].mask = atk_mask;

            /* brute force the magic number */
            for (;;) {
                Bb64 magic = my_rand64() & my_rand64() & my_rand64();

                Bb64 atk = 0ULL;
                for (Bb64 test = 0; test < (1ULL<<rook_relevant_bits_count); test++) {
                    /* map test to an occupany set */
                    Bb64 occ = 0ULL;
                    for (size_t i = 0; i < rook_relevant_bits_count; i++) {
                        occ |= ((test >> i) & 1ULL) << rook_relevant_bits[i];
                    }

                    atk = rook_attacks_from_index(sq_index, occ);

                    const size_t hash = ((occ * magic) >> (64ULL-12));

                    if (rook_attacks[sq_index][hash] == 0ULL) {
                        rook_attacks[sq_index][hash] = atk;
                    } else if (rook_attacks[sq_index][hash] != atk) {
                        for (size_t i = 0; i < sizeof rook_attacks[0] / sizeof rook_attacks[0][0]; i++) {
                            rook_attacks[sq_index][i] = 0ULL;
                        }
                        goto next_rook_magic;
                    }
                }

                rook_magics[sq_index].magic = magic;
                break;

    next_rook_magic:
            }
        }


        /* BISHOPS
         * ====================== */
        {
            /* generate attack mask */
            Bb64 atk_mask = 0;
            {
                atk_mask |= diagonals_from_index(sq_index);
                atk_mask &= ~(RANK_MASK_1 | RANK_MASK_8);
                atk_mask &= ~(FILE_MASK_A | FILE_MASK_H);
                atk_mask &= ~MASK_FROM_RF(rank, file);

                /* populate relevant bits array */
                bishop_relevant_bits_count = 0;
                Bb64 x = atk_mask;
                while (x) {
                    Sq8 const lsb = bitboard_pop_lsb(&x);
                    bishop_relevant_bits[bishop_relevant_bits_count++] = lsb;
                }
            }
            bishop_magics[sq_index].mask = atk_mask;

            /* brute force the magic number */
            Bb64 magic = 0ULL;
            for (;;) {
                magic = my_rand64() & my_rand64() & my_rand64();

                Bb64 atk = 0ULL;
                for (Bb64 test = 0; test < (1ULL<<bishop_relevant_bits_count); test++) {
                    /* map test to an occupany set */
                    Bb64 occ = 0ULL;
                    for (size_t i = 0; i < bishop_relevant_bits_count; i++) {
                        occ |= ((test >> i) & 1ULL) << bishop_relevant_bits[i];
                    }

                    atk = bishop_attacks_from_index(sq_index, occ);

                    const size_t hash = ((occ * magic) >> (64ULL-9));

                    if (bishop_attacks[sq_index][hash] == 0ULL) {
                        bishop_attacks[sq_index][hash] = atk;
                    } else if (bishop_attacks[sq_index][hash] != atk) {
                        for (size_t i = 0; i < sizeof bishop_attacks[0] / sizeof bishop_attacks[0][0]; i++) {
                            bishop_attacks[sq_index][i] = 0ULL;
                        }
                        goto next_bishop_magic;
                    }
                }
                bishop_magics[sq_index].magic = magic;
                break;

    next_bishop_magic:
            }
        }

        /* BETWEEN TABLE
         * ===================== */
        {
            for (Sq8 i = SQ_BEGIN; i < SQ_COUNT; ++i) {
                for (Sq8 j = SQ_BEGIN; j < SQ_COUNT; ++j) {
                    between_lookup[i][j] = between_mask(i, j);
                }
            }
        }
   }

    /*  --- write to files ---  */
#define TAB "    "
#define NL "\n"

    { /* diagonals */
        FILE* f = fopen("diagonals.h", "w");
        if (!f) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        fprintf(f, "static const Bb64 diagonals[SQ_COUNT] = {\n");
        for (Sq8 i = SQ_BEGIN; i < SQ_COUNT; ++i) {
            fprintf(f, "[%s] = 0x%016"BITBOARD_FMT_X",\n",
                    sq8_str[i],
                    diagonals[i]);
        }
        fprintf(f,"};\n");
        fclose(f);
    }

    { /* rooks */
        FILE* f = fopen("mbb_rook.h", "w");
        if (!f) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        fprintf(f, "#pragma once\n");
        fprintf(f, "static const struct magic mbb_rook[SQ_COUNT] = {\n");
        for (Sq8 i = SQ_BEGIN; i < SQ_COUNT; i++) {
            fprintf(f,
                 TAB "[%s] = (struct magic) {"
              NL TAB "    .magic = 0x%016"BITBOARD_FMT_X"ULL,"
              NL TAB "    .mask  = 0x%016"BITBOARD_FMT_X"ULL,"
              NL TAB "},"
              NL,
              sq8_str[i],
              rook_magics[i].magic,
              rook_magics[i].mask);
        }
        fprintf(f,"};\n");

        fprintf(f,"\n");

        fprintf(f, "static const Bb64 rook_attacks[SQ_COUNT][1ULL<<12ULL] = {\n");
        for (Sq8 sq = SQ_BEGIN; sq < SQ_COUNT; ++sq) {
            fprintf(f, "[%s] = {\n", sq8_str[sq]);
            for (size_t i = 0; i < sizeof rook_attacks[sq] / sizeof *rook_attacks[sq]; i++) {
                fprintf(f, "0x%016"BITBOARD_FMT_X"ULL, \n", rook_attacks[sq][i]);
            }
            fprintf(f, "\n},\n");
        }
        fprintf(f,"};\n");

        fclose(f);
    }

    { /* bishops */
        FILE* f = fopen("mbb_bishop.h", "w");
        if (!f) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        fprintf(f, "\n");
        fprintf(f, "static const struct magic mbb_bishop[SQ_COUNT] = {\n");
        for (Sq8 i = SQ_BEGIN; i < SQ_COUNT; i++) {
            fprintf(f,
                 TAB "[%s] = (struct magic) {"
              NL TAB "    .magic = 0x%016"BITBOARD_FMT_X"ULL,"
              NL TAB "    .mask  = 0x%016"BITBOARD_FMT_X"ULL,"
              NL TAB "},"
              NL,
              sq8_str[i],
              bishop_magics[i].magic,
              bishop_magics[i].mask);
        }
        fprintf(f,"};\n");

        fprintf(f,"\n");

        fprintf(f, "static const Bb64 bishop_attacks[SQ_COUNT][1ULL<<9ULL] = {\n");
        for (Sq8 sq = SQ_BEGIN; sq < SQ_COUNT; ++sq) {
            fprintf(f, "[%s] = {\n", sq8_str[sq]);
            for (size_t i = 0; i < sizeof bishop_attacks[sq] / sizeof *bishop_attacks[sq]; i++) {
                fprintf(f, "0x%016"BITBOARD_FMT_X"ULL, \n", bishop_attacks[sq][i]);
            }
            fprintf(f, "\n},\n");
        }
        fprintf(f,"};\n");
        fclose(f);
    }

    { /* between table */
        FILE* f = fopen("between_lookup.h", "w");
        if (!f) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        fprintf(f, "static const Bb64 between_lookup[SQ_COUNT][SQ_COUNT] = {\n");
        for (Sq8 i = SQ_BEGIN; i < SQ_COUNT; ++i) {
            fprintf(f, "[%s] = {\n", sq8_str[i]);
            for (Sq8 j = SQ_BEGIN; j < SQ_COUNT; ++j) {
                fprintf(f, "0x%016"BITBOARD_FMT_X"ULL, \n", between_lookup[i][j]);
            }
            fprintf(f, "\n},\n");
        }
        fprintf(f,"};\n");
        fclose(f);
    }

#undef TAB
#undef NL
}
