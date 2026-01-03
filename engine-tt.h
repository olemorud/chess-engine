#pragma once

struct search_option {
    /* TODO: optimize order of fields and size */
    double      score;
    struct move move;
    uint64_t    hash;
    uint8_t     init : 1;
    enum tt_flag {TT_EXACT, TT_LOWER, TT_UPPER} flag : 3;
    int8_t      depth : 4;
};

#define TT_ADDRESS_BITS 24
#define TT_ENTRIES (1ULL<<TT_ADDRESS_BITS)
struct tt {
    struct search_option* entries/*[TT_ENTRIES]*/; /* must be initialized somewhere */

#ifndef NDEBUG
    /* stats */
    uint64_t collisions;
    uint64_t hits;
    uint64_t probes;
    uint64_t overwritten;
    uint64_t insertions;
#endif
};

struct zobrist {
    uint64_t piece_keys[SQ_COUNT][SIDE_COUNT][PIECE_COUNT];
    uint64_t ep_targets[SQ_COUNT];
    uint64_t castling_keys[SIDE_COUNT][CASTLE_COUNT];
    bool init;
};

static struct zobrist zobrist;

static void init_zobrist()
{
    for (Sq8 sq = SQ_BEGIN; sq < SQ_COUNT; ++sq) {
        for (Side8 pl = SIDE_BEGIN; pl < SIDE_COUNT; ++pl) {
            for (Piece8 piece = PIECE_BEGIN; piece < PIECE_COUNT; ++piece) {
                zobrist.piece_keys[sq][pl][piece] = my_rand64();
            }
        }
        zobrist.ep_targets[sq] = my_rand64();
    }
    for (Side8 pl = SIDE_BEGIN; pl < SIDE_COUNT; ++pl) {
        for (enum castle_direction d = CASTLE_BEGIN; d < CASTLE_COUNT; ++d) {
            zobrist.castling_keys[pl][d] = my_rand64();
        }
    }
    zobrist.init = true;
}

static inline uint64_t tt_hash_update(uint64_t hash,
                                      Sq8      sq,
                                      Side8    us,
                                      Piece8   piece)
{
    if (!zobrist.init) {
        init_zobrist();
    }
    return hash ^ zobrist.piece_keys[sq][us][piece];
}

static inline uint64_t tt_hash_update_castling_rights(uint64_t              hash,
                                                      Side8           us,
                                                      enum castle_direction dir)
{
    if (!zobrist.init) {
        init_zobrist();
    }
    return hash ^ zobrist.castling_keys[us][dir];
}

static inline uint64_t tt_hash_update_ep_targets(uint64_t hash, Sq8 sq)
{
    if (!zobrist.init) {
        init_zobrist();
    }
    assuming(sq < SQ_COUNT);
    return hash ^ zobrist.ep_targets[sq];
}

static inline uint64_t tt_hash_switch_side(uint64_t hash)
{
    if (!zobrist.init) {
        init_zobrist();
    }
    return ~hash;
}

static inline struct search_option tt_get(struct tt* tt, uint64_t hash)
{
    uint64_t const addr = hash % TT_ENTRIES;
    struct search_option tte = tt->entries[addr];

#ifndef NDEBUG
    tt->probes += 1;
    if (tte.init && tte.hash == hash) {
        tt->hits += 1;
    } else if (tte.init && tte.hash != hash) {
        tt->collisions += 1;
    }
#endif
    return tte;
}

static inline void tt_insert(struct tt* tt, uint64_t hash, struct search_option so)
{
    uint64_t const addr = hash % TT_ENTRIES;
    so.init = true;
    tt->entries[addr] = so;
#ifndef NDEBUG
    tt->insertions += 1;
#endif
}

/* tt_insert_maybe inserts only if heuristics say it's a good idea. There are
 * two considerations:
 * - higher depth saves more work per probe hit
 * - entries closer to the leaves are more likely to be searched multiple time
 */
static inline void tt_insert_maybe(struct tt* tt, uint64_t hash, struct search_option so)
{
    uint64_t const addr = hash % TT_ENTRIES;

#if 0
    struct search_option* tte = &tt->entries[addr];
    if (so.depth < tte->depth) {
        *tte = so;
    }
#endif

    so.init = true;
    tt->entries[addr] = so;
#ifndef NDEBUG
    tt->insertions += 1;
#endif
}

