#define CODEGEN
#include "engine.h"
#include "libc-lite.h"
#include "board_print.h"
#include <stdlib.h>
#include <stdio.h>

/* gets the next number with the same bitcount, unless it overflows, in which
 * it returns the first number with popcount+1 */
uint64_t next_magic(uint64_t seed) {

    uint64_t u = seed & -seed; // rightmost 1-bit
    uint64_t v = seed + u;     // add it
    if (v == 0) {
        if (seed == ~0ULL) {
            return 0;
        }
        return next_magic((1ULL << (popcount(seed)+1)) - 1);
    }
    seed = v + (((v ^ seed) / u) >> 2);

    return seed;
}


int main()
{
    struct magic rook_magics[SQ_INDEX_COUNT] = {0ULL};
    bitboard rook_attacks[SQ_INDEX_COUNT][1<<12ULL] = {0};

    index rook_relevant_bits[12] = {0};
    size_t rook_relevant_bits_count = 0;

    struct magic bishop_magics[SQ_INDEX_COUNT] = {0ULL};
    bitboard bishop_attacks[SQ_INDEX_COUNT][1<<9ULL] = {0};

    index bishop_relevant_bits[9] = {0};
    size_t bishop_relevant_bits_count = 0;

    bitboard between_lookup[SQ_INDEX_COUNT][SQ_INDEX_COUNT];

    bitboard diagonals[SQ_INDEX_COUNT];

    for (enum square_index sq = SQ_INDEX_BEGIN; sq < SQ_INDEX_COUNT; ++sq) {
        diagonals[sq] = diagonals_from_index(sq);
    }

    for (enum square_index sq_index = SQ_INDEX_BEGIN; sq_index < SQ_INDEX_COUNT; ++sq_index) {
        enum file_index file = index_to_file(sq_index);
        enum rank_index rank = index_to_rank(sq_index);
        fprintf(stderr, "%s ", square_index_display[sq_index]);

        /* ROOKS
         * ======================= */
        {
            /* generate attack mask */
            bitboard atk_mask = 0;
            {
                atk_mask |= FILE_MASK(file) & ~(RANK_MASK_1 | RANK_MASK_8);
                atk_mask |= RANK_MASK(rank) & ~(FILE_MASK_A | FILE_MASK_H);
                atk_mask &= ~SQ_MASK_FROM_RF(rank, file);

                /* populate relevant bits array */
                rook_relevant_bits_count = 0;
                bitboard x = atk_mask;
                while (x) {
                    const index lsb = bitboard_pop_lsb(&x);
                    rook_relevant_bits[rook_relevant_bits_count++] = lsb;
                }
            }
            rook_magics[sq_index].mask = atk_mask;

            /* brute force the magic number */
            for (;;) { 
                bitboard magic = my_rand64() & my_rand64() & my_rand64();

                bitboard atk = 0ULL;
                for (bitboard test = 0; test < (1ULL<<rook_relevant_bits_count); test++) {
                    /* map test to an occupany set */
                    bitboard occ = 0ULL;
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
            bitboard atk_mask = 0;
            {
                atk_mask |= diagonals_from_index(sq_index);
                atk_mask &= ~(RANK_MASK_1 | RANK_MASK_8);
                atk_mask &= ~(FILE_MASK_A | FILE_MASK_H);
                atk_mask &= ~SQ_MASK_FROM_RF(rank, file);

                /* populate relevant bits array */
                bishop_relevant_bits_count = 0;
                bitboard x = atk_mask;
                while (x) {
                    const index lsb = bitboard_pop_lsb(&x);
                    bishop_relevant_bits[bishop_relevant_bits_count++] = lsb;
                }
            }
            bishop_magics[sq_index].mask = atk_mask;

            /* brute force the magic number */
            bitboard magic = 0ULL;
            for (;;) { 
                magic = my_rand64() & my_rand64() & my_rand64();

                bitboard atk = 0ULL;
                for (bitboard test = 0; test < (1ULL<<bishop_relevant_bits_count); test++) {
                    /* map test to an occupany set */
                    bitboard occ = 0ULL;
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
            for (enum square_index i = SQ_INDEX_BEGIN; i < SQ_INDEX_COUNT; ++i) {
                for (enum square_index j = SQ_INDEX_BEGIN; j < SQ_INDEX_COUNT; ++j) {
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

        fprintf(f, "static const bitboard diagonals[SQ_INDEX_COUNT] = {\n");
        for (enum square_index i = SQ_INDEX_BEGIN; i < SQ_INDEX_COUNT; ++i) {
            fprintf(f, "[%s] = 0x%016"BITBOARD_FMT_X",\n",
                    square_index_str[i],
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
        fprintf(f, "static const struct magic mbb_rook[SQ_INDEX_COUNT] = {\n");
        for (enum square_index i = SQ_INDEX_BEGIN; i < SQ_INDEX_COUNT; i++) {
            fprintf(f,
                 TAB "[%s] = (struct magic) {"
              NL TAB "    .magic = 0x%016"BITBOARD_FMT_X"ULL,"
              NL TAB "    .mask  = 0x%016"BITBOARD_FMT_X"ULL,"
              NL TAB "},"
              NL,
              square_index_str[i],
              rook_magics[i].magic,
              rook_magics[i].mask);
        }
        fprintf(f,"};\n");

        fprintf(f,"\n");

        fprintf(f, "static const bitboard rook_attacks[SQ_INDEX_COUNT][1ULL<<12ULL] = {\n");
        for (enum square_index sq = SQ_INDEX_BEGIN; sq < SQ_INDEX_COUNT; ++sq) {
            fprintf(f, "[%s] = {\n", square_index_str[sq]);
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
        fprintf(f, "static const struct magic mbb_bishop[SQ_INDEX_COUNT] = {\n");
        for (enum square_index i = SQ_INDEX_BEGIN; i < SQ_INDEX_COUNT; i++) {
            fprintf(f,
                 TAB "[%s] = (struct magic) {"
              NL TAB "    .magic = 0x%016"BITBOARD_FMT_X"ULL,"
              NL TAB "    .mask  = 0x%016"BITBOARD_FMT_X"ULL,"
              NL TAB "},"
              NL,
              square_index_str[i],
              bishop_magics[i].magic,
              bishop_magics[i].mask);
        }
        fprintf(f,"};\n");

        fprintf(f,"\n");

        fprintf(f, "static const bitboard bishop_attacks[SQ_INDEX_COUNT][1ULL<<9ULL] = {\n");
        for (enum square_index sq = SQ_INDEX_BEGIN; sq < SQ_INDEX_COUNT; ++sq) {
            fprintf(f, "[%s] = {\n", square_index_str[sq]);
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

        fprintf(f, "static const bitboard between_lookup[SQ_INDEX_COUNT][SQ_INDEX_COUNT] = {\n");
        for (enum square_index i = SQ_INDEX_BEGIN; i < SQ_INDEX_COUNT; ++i) {
            fprintf(f, "[%s] = {\n", square_index_str[i]);
            for (enum square_index j = SQ_INDEX_BEGIN; j < SQ_INDEX_COUNT; ++j) {
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
