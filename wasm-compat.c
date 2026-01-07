/* The purpose of this file is to be a simple wasm helper layer, all state is global */

#include "engine.h"

#include <stdint.h>
#include <limits.h>

static struct search_option g_tt_buf[TT_ENTRIES];
static struct board         g_board;
static uint16_t             g_legal_moves_buf[MOVE_MAX];
static uint32_t             g_legal_moves_len;

void wb_init(void)
{
    g_board = BOARD_INIT;
    g_board.tt.entries = g_tt_buf;
    g_board.tt.mask = TT_MASK;
    board_init(&g_board);
}

static inline uint32_t move_serialize(struct move m)
{
    _Static_assert(sizeof m.from * CHAR_BIT == 8,
            "this must be checked if struct move's `from` changes");
    _Static_assert(sizeof m.to * CHAR_BIT == 8,
            "this must be checked if struct move's `to` changes");

    return ((uint32_t)m.appeal << 24U)
         | ((uint32_t)m.attr   << 16U)
         | ((uint32_t)m.from   <<  8U)
         | ((uint32_t)m.to     <<  0U);
}

static inline struct move move_deserialize(uint32_t m)
{
    return (struct move) {
        /* appeal and attributes are ignored regardless */
        /*
        .appeal = (m >> 24) & 0xFF,
        .attr   = (m >> 16) & 0xFF,
        */
        .from   = (m >>  8) & 0xFF,
        .to     = (m >>  0) & 0xFF,
    };
}

uint64_t wb_search(int8_t max_depth)
{
    struct search_result const sr = search(&g_board, g_board.pos.moving_side, max_depth);
    return (uint64_t)move_serialize(sr.move);
}

int32_t wb_move(uint32_t move)
{
    struct move const m = move_deserialize(move);
    enum move_result const mr = board_move(&g_board, m);

    /* TODO: this checkmate/stalemate check needs to be abstracted better */
    if (mr == MR_STALEMATE) {
        return (int32_t)MR_STALEMATE;
    }
    struct move moves[MOVE_MAX];
    size_t move_count = 0ULL;
    all_pseudolegal_moves(&g_board.pos, MG_ALL, other_side(g_board.pos.moving_side), &move_count, moves);
    if (move_count == 0ULL) {
        return (int32_t)MR_CHECKMATE;
    }

    return (int32_t)mr;
}


static int32_t side_piece_serialize(Side8 c, Piece8 pz)
{
    return ((c  & 0xFF) << 8U)
         |  (pz & 0xFF);
}

int32_t wb_board_at(uint8_t at)
{
    Bb64 const m = MASK_FROM_SQ((Sq8)at);
    for (Side8 side = SIDE_BEGIN; side < SIDE_COUNT; ++side) {
        for (Piece8 pz = PIECE_BEGIN; pz < PIECE_COUNT; ++pz) {
            if (g_board.pos.pieces[side][pz] & m) {
                return side_piece_serialize(side, pz);
            }
        }
    }
	return -1;
}

uint32_t wb_all_moves_len(void)
{
    struct move moves[MOVE_MAX];
    size_t cnt = 0;
    all_pseudolegal_moves(&g_board.pos, MG_ALL, g_board.pos.moving_side, &cnt, moves);

    if (cnt > MOVE_MAX) {
        cnt = MOVE_MAX;
    }

    g_legal_moves_len = (uint32_t)cnt;
    for (size_t i = 0; i < cnt; ++i) {
        g_legal_moves_buf[i] = (uint16_t)(((uint32_t)moves[i].from << 8U)
                                        |  (uint32_t)moves[i].to);
    }

    return g_legal_moves_len;
}

uint32_t wb_all_moves_get(uint32_t i)
{
    if (i >= g_legal_moves_len) return 0U;
    return (uint32_t)g_legal_moves_buf[i];
}

