
#include "engine.h"

#include <stdint.h>

static struct search_option g_tt_buf[TT_ENTRIES];

static struct board g_board;

static inline uint32_t move_serialize(struct move m) 
{
	_Static_assert(sizeof m.from * CHAR_BIT == 8,
			"this must be checked if struct move's `from` changes");
	_Static_assert(sizeof m.to * CHAR_BIT == 8,
			"this must be checked if struct move's `to` changes");
	return ((uint32_t)m.appeal << 24ULL)
		 | ((uint32_t)m.attr << 16ULL)
		 | ((uint32_t)m.from << 8ULL)
		 | ((uint32_t)m.to);
}

static inline struct move move_deserialize(uint64_t m)
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
	struct search_result const sr = search(&g_board, g_board.pos.player, max_depth);
	return move_serialize(sr.move);
}

int32_t wb_move(uint32_t move)
{
	struct move m = move_deserialize(move);
	enum move_result const r = board_move_2(&g_board, m);
	return (int32_t)r;
}

void wb_init()
{
	g_board = BOARD_INIT;
	g_board.tt.entries = g_tt_buf;
}

static int32_t player_piece_serialize(enum player c, enum piece pz)
{
	return ((c  & 0xFF) << 8U)
		 |  (pz & 0xFF);
}

int32_t wb_board_at(index at)
{
	bitboard const m = SQ_MASK_FROM_INDEX(at);
	for (enum player pl = PLAYER_BEGIN; pl < PLAYER_COUNT; ++pl) {
		for (enum piece pz = PIECE_BEGIN; pz < PIECE_COUNT; ++pz) {
			if (g_board.pos.pieces[pl][pz] & m) {
				return player_piece_serialize(pl, pz);
			}
		}
	}
	return -1;
}
