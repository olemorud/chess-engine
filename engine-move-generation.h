#pragma once

#define MOVE_CASTLE_KINGSIDE_WHITE (struct move) \
    {.from = SQ_E1, .to = SQ_G1}

#define MOVE_CASTLE_QUEENSIDE_WHITE (struct move) \
    {.from = SQ_E1, .to = SQ_C1}

#define MOVE_CASTLE_KINGSIDE_BLACK (struct move) \
    {.from = SQ_E8, .to = SQ_G8}

#define MOVE_CASTLE_QUEENSIDE_BLACK (struct move) \
    {.from = SQ_E8, .to = SQ_C8}

static inline
struct move move_make(struct pos const* restrict pos,
                      Piece8 piece,
                      Index8 from,
                      Index8 to,
                      uint8_t add_attr)
{
    (void)piece;
    (void)pos;
    (void)add_attr;
#if 0
    Side8 const us = pos->moving_side;
    Side8 const them = other_side(us);
    Bb64 const their_occ = pos->occupied[them];
    Bb64 const tomask = MASK_FROM_SQ(to);
    Bb64 const finishline = (us == SIDE_WHITE ? RANK_MASK_8 : RANK_MASK_1);

    uint8_t attr = 0ULL;
#define MASK_IF8(x) ((~(uint8_t)0U) + (uint8_t)!(x))
    attr |= MATTR_CAPTURE & MASK_IF8(tomask & their_occ);
    attr |= MATTR_CAPTURE & MASK_IF8((piece == PIECE_PAWN) && tomask & pos->ep_targets);
    attr |= MATTR_PROMOTE & MASK_IF8((piece == PIECE_PAWN) && (tomask & finishline));
    attr |= add_attr;
#undef MASK_IF8

    return (struct move){.from = from, .to = to, .attr = attr};
#endif
    return (struct move){.from = from, .to = to, .attr = add_attr};
}
#define MOVE_MAX 128

enum move_gen_type {
    MG_ALL,
    MG_CAPTURES,
    MG_CHECKS,
    MG_QUIETS,
};

static void all_pseudolegal_from_piece(struct pos const* restrict pos,
                                       enum move_gen_type         type,
                                       Piece8                     piece,
                                       Side8                      us,
                                       Bb64                       piece_mask,
                                       Bb64                       allowed,
                                       size_t* restrict           out_count,
                                       struct move                out[restrict static MOVE_MAX])
{
    assuming(piece != PIECE_PAWN);

    Side8 them = other_side(us);

    Bb64 const our_occ   = pos->occupied[us];
    Bb64 const all_occ   = pos->occupied[SIDE_WHITE] | pos->occupied[SIDE_BLACK];

    if (type == MG_CHECKS) {
        allowed &= non_pawn_piece_attacks(piece, pos->pieces[them][PIECE_KING], all_occ);
    }

    /* pray this gets constprop'ed */
    while (piece_mask) {
        Sq8 from = bitboard_pop_lsb(&piece_mask);
        Bb64 move_mask = non_pawn_piece_attacks_from_index(piece, from, all_occ)
                       & ~our_occ
                       & allowed
                       ;
        while (move_mask) {
            Sq8 const to = bitboard_pop_lsb(&move_mask);
            assuming(*out_count < MOVE_MAX); \
            out[(*out_count)++] = move_make(pos, piece, from, to, 0);
        }
    }
}

#define DEFINE_ALL_PSEUDOLEGAL_PAWN_MOVES(side, opp_side, side_enum, inverse_direction, pawn_rank) \
static void all_pseudolegal_pawn_moves_##side(struct pos const* restrict pos,\
                                              enum move_gen_type         type,\
                                              Bb64                       piece_mask,\
                                              Bb64                       allowed,\
                                              size_t* restrict           out_count,\
                                              struct move                out[restrict static MOVE_MAX])\
{\
    Bb64 const all_occ = pos->occupied[SIDE_WHITE] | pos->occupied[SIDE_BLACK];\
\
    if (type == MG_CHECKS) {\
        allowed &= pawn_attacks_##opp_side(pos->pieces[other_side(side_enum)][PIECE_KING]);\
    }\
    Bb64 sp = pawn_single_push_##side(piece_mask, ~all_occ)\
               & allowed\
               ;\
    while (sp) {\
        Sq8 const to = bitboard_pop_lsb(&sp);\
        Sq8 const from = SQ_SHIFT_##inverse_direction(to, 1);\
        out[(*out_count)++] = move_make(pos, PIECE_PAWN, from, to, 0);\
    }\
\
    Bb64 dp = pawn_double_push_##side(piece_mask & pawn_rank, ~all_occ)\
               & allowed\
               ;\
    while (dp) {\
        Sq8 const to = bitboard_pop_lsb(&dp);\
        Sq8 const from = SQ_SHIFT_##inverse_direction(to, 2);\
        out[(*out_count)++] = move_make(pos, PIECE_PAWN, from, to, 0);\
    }\
}
DEFINE_ALL_PSEUDOLEGAL_PAWN_MOVES(white, black, SIDE_WHITE, SOUTH, RANK_MASK_2)
DEFINE_ALL_PSEUDOLEGAL_PAWN_MOVES(black, white, SIDE_BLACK, NORTH, RANK_MASK_7)

#define DEFINE_ALL_PSEUDOLEGAL_PAWN_ATTACKS(side, opp_side, side_enum, inverse_direction) \
static void all_pseudolegal_pawn_attacks_##side(struct pos const* restrict pos,\
                                                enum move_gen_type         type,\
                                                Bb64                       piece_mask,\
                                                Bb64                       allowed,\
                                                size_t* restrict           out_count,\
                                                struct move                out[restrict static MOVE_MAX])\
{\
    Bb64 const their_occ = pos->occupied[other_side(side_enum)];\
\
    if (type == MG_CHECKS) {\
        allowed &= pawn_attacks_##opp_side(pos->pieces[other_side(side_enum)][PIECE_KING]);\
    }\
\
    Bb64 ratk = pawn_attacks_right_##side(piece_mask)\
              & (their_occ | pos->ep_targets)\
              & allowed\
              ;\
    while (ratk) {\
        Sq8 const to = bitboard_pop_lsb(&ratk);\
        Sq8 const from = SQ_SHIFT_WEST(SQ_SHIFT_##inverse_direction(to, 1), 1);\
        assuming(*out_count < MOVE_MAX); \
        out[(*out_count)++] = move_make(pos, PIECE_PAWN, from, to, 0);\
    }\
\
    Bb64 latk = pawn_attacks_left_##side(piece_mask)\
              & (their_occ | pos->ep_targets)\
              & allowed\
              ;\
    while (latk) {\
        Sq8 const to = bitboard_pop_lsb(&latk);\
        Sq8 const from = SQ_SHIFT_EAST(SQ_SHIFT_##inverse_direction(to, 1), 1);\
        assuming(*out_count < MOVE_MAX); \
        out[(*out_count)++] = move_make(pos, PIECE_PAWN, from, to, 0);\
    }\
}
DEFINE_ALL_PSEUDOLEGAL_PAWN_ATTACKS(white, black, SIDE_WHITE, SOUTH)
DEFINE_ALL_PSEUDOLEGAL_PAWN_ATTACKS(black, white, SIDE_BLACK, NORTH)

static void all_pseudolegal_moves(struct pos const* restrict pos,
                                  enum move_gen_type         type,
                                  Side8                      us,
                                  size_t* restrict           out_count,
                                  struct move                out[restrict static MOVE_MAX])
{
    Side8 const them = other_side(us);
    if (type == MG_CHECKS && pos->pieces[them][PIECE_KING] == 0ULL) {
        return;
    }
    Bb64 const kmask = pos->pieces[us][PIECE_KING];
    assuming(kmask);
    Sq8 const ksq = bitboard_lsb(kmask);


    Bb64 const chk = checkers(pos, us);

    Bb64 const their_threats = all_threats_from_side(pos, them);

    Bb64 const their_occ = pos->occupied[them];
    Bb64 const all_occ   = pos->occupied[SIDE_WHITE] | pos->occupied[SIDE_BLACK];

    Bb64 allowed;
    if (type == MG_CAPTURES) {
        allowed = their_occ;
    } else if (type == MG_QUIETS) {
        allowed = ~their_occ;
    } else if (type == MG_ALL) {
        allowed = ~0ULL;
    } else {
        allowed = ~0ULL;
    }

    Bb64 king_allowed = allowed;

    if (chk) {
        if (bitboard_more_than_one(chk)) {
            if (type == MG_ALL) {
                /* no other piece can fix check if the king has two attackers or more */
                all_pseudolegal_from_piece(pos, type, PIECE_KING, us, pos->pieces[us][PIECE_KING], ~their_threats, out_count, out);
            }
            return;
        }

        Sq8 const asq = bitboard_lsb(chk);
        Bb64 const blocking_mask = between_mask(ksq, asq);
        allowed &= blocking_mask | chk;
    }

    if (type == MG_CAPTURES || type == MG_CHECKS || type == MG_ALL) {
        Bb64 const x = (type == MG_CHECKS ? pos->ep_targets : 0ULL);
        if (us == SIDE_WHITE) {
            all_pseudolegal_pawn_attacks_white(pos, type, pos->pieces[us][PIECE_PAWN], allowed | x, out_count, out);
        } else {
            all_pseudolegal_pawn_attacks_black(pos, type, pos->pieces[us][PIECE_PAWN], allowed | x, out_count, out);
        }
    }

    if (type == MG_QUIETS || type == MG_CHECKS || type == MG_ALL) {
        if (us == SIDE_WHITE) {
            all_pseudolegal_pawn_moves_white(pos, type, pos->pieces[us][PIECE_PAWN], allowed, out_count, out);
        } else {
            all_pseudolegal_pawn_moves_black(pos, type, pos->pieces[us][PIECE_PAWN], allowed, out_count, out);
        }
    }

    all_pseudolegal_from_piece(pos, type, PIECE_BISHOP, us, pos->pieces[us][PIECE_BISHOP], allowed, out_count, out);
    all_pseudolegal_from_piece(pos, type, PIECE_KNIGHT, us, pos->pieces[us][PIECE_KNIGHT], allowed, out_count, out);
    all_pseudolegal_from_piece(pos, type, PIECE_ROOK,   us, pos->pieces[us][PIECE_ROOK],   allowed, out_count, out);
    all_pseudolegal_from_piece(pos, type, PIECE_QUEEN,  us, pos->pieces[us][PIECE_QUEEN],  allowed, out_count, out);

    all_pseudolegal_from_piece(pos, type, PIECE_KING,   us, pos->pieces[us][PIECE_KING], ~their_threats & king_allowed, out_count, out);

    if (type == MG_CHECKS) {
        Bb64 discovering = pinning_lines_to_target(pos, us, bitboard_lsb(pos->pieces[them][PIECE_KING]));

        all_pseudolegal_from_piece(pos, MG_ALL, PIECE_BISHOP, us, discovering & pos->pieces[us][PIECE_BISHOP], ~discovering, out_count, out);
        all_pseudolegal_from_piece(pos, MG_ALL, PIECE_KNIGHT, us, discovering & pos->pieces[us][PIECE_KNIGHT], ~discovering, out_count, out);
        all_pseudolegal_from_piece(pos, MG_ALL, PIECE_ROOK,   us, discovering & pos->pieces[us][PIECE_ROOK],   ~discovering, out_count, out);
        all_pseudolegal_from_piece(pos, MG_ALL, PIECE_QUEEN,  us, discovering & pos->pieces[us][PIECE_QUEEN],  ~discovering, out_count, out);
        all_pseudolegal_from_piece(pos, MG_ALL, PIECE_KING,   us, discovering & pos->pieces[us][PIECE_KING],   ~discovering & ~their_threats, out_count, out);
    }

    /* castling */
    if (!chk && type != MG_CAPTURES) {
        bool can_castle_kingside, can_castle_queenside;
        Bb64 const blocked = pos->occupied[SIDE_WHITE] | pos->occupied[SIDE_BLACK] | their_threats;
        if (us == SIDE_WHITE) {
            can_castle_kingside = !(blocked & (SQMASK_F1 | SQMASK_G1))
                          && (pos->pieces[us][PIECE_ROOK] & SQMASK_H1)
                          && !pos->castling_illegal[us][CASTLE_KINGSIDE];
            can_castle_queenside = !(blocked & (SQMASK_C1 | SQMASK_D1))
                          && (pos->pieces[us][PIECE_ROOK] & SQMASK_A1)
                          && !pos->castling_illegal[us][CASTLE_QUEENSIDE];
        } else {
            can_castle_kingside  = !(blocked & (SQMASK_F8 | SQMASK_G8))
                          && (pos->pieces[us][PIECE_ROOK] & SQMASK_H8)
                          && !pos->castling_illegal[us][CASTLE_KINGSIDE];
            can_castle_queenside = !(blocked & (SQMASK_C8 | SQMASK_D8))
                          && (pos->pieces[us][PIECE_ROOK] & SQMASK_A8)
                          && !pos->castling_illegal[us][CASTLE_QUEENSIDE];
        }

        if (can_castle_kingside) {
            out[(*out_count)++] = us == SIDE_WHITE
                                  ? MOVE_CASTLE_KINGSIDE_WHITE
                                  : MOVE_CASTLE_KINGSIDE_BLACK;
        }

        if (can_castle_queenside) {
            out[(*out_count)++] = us == SIDE_WHITE
                                  ? MOVE_CASTLE_QUEENSIDE_WHITE
                                  : MOVE_CASTLE_QUEENSIDE_BLACK;
        }
    }
}
