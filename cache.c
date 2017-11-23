#include <assert.h>
#include "sbv-imp.h"
#include "encode.h"

static void cache_invalidate(struct sbv_t *sbv) {
  for (int i = 0; i < WAYS; ++i) {
    sbv->cache.ways[i].size1 = 0;
    sbv->cache.ways[i].age   = 0;
  }
}

void cache_init(struct sbv_t *sbv) {
  sbv->cache.counter = 0;
  cache_invalidate(sbv);
}

// index of first offset >= k; returns size if not found
static int binary_search_index(uint64_t *offsets, uint64_t size, uint64_t k) {
  int low = 0;
  int high = size;

  while (low != high) {
    int mid = (low+high)/2; // never inspects high
    if (offsets[mid] < k) {
      low = mid+1;
    } else {
      high = mid;
    }
  }

  return low;
}

struct find_rank_context_t {
  offset_t k;
  offset_t *offsets;
  rank_t fill;
  rank_t r;
};

static void find_rank(void *_context, uint64_t rank, uint64_t value) {
  struct find_rank_context_t *context = _context;
  if (context->fill > 0) {
    if (context->fill < DEPTH) {
      context->offsets[context->fill] = value;
      ++context->fill;
    }
  } else if (value >= context->k) {
    context->r = rank;
    context->fill = 1;
    context->offsets[0] = value;
  }
}

rank_t cache_rank1(struct sbv_t *sbv, offset_t k) {
  struct meta_t *meta = &sbv->cache.ways[0];
  uint32_t op = ++sbv->cache.counter;
  uint32_t age = ~UINT32_C(0);
  int oldest;

  // counter overflow starts a new cache generation
  if (!op) cache_invalidate(sbv);

  for (int i = 0; i < WAYS; ++i) {
    if (meta[i].size1 && meta[i].lower <= k && k <= meta[i].upper) {
      meta[i].age = op;
      return meta[i].rank1 +
        binary_search_index(&sbv->cache.offsets[i][0], meta[i].size1, k);
    }
    if (age >= meta[i].age) {
      oldest = i;
      age = meta[i].age;
    }
  }

  struct btree_result leaf = btree_rank1(sbv, k);
  assert (leaf.lower <= k && k <= leaf.upper);
  if (leaf.size1 == 0) return leaf.rank1;

  uint64_t *page = block_get(sbv, leaf.page);
  struct find_rank_context_t context = { k, &sbv->cache.offsets[oldest][0], 0, 0 };
  decode(leaf.lower, leaf.upper, leaf.rank1, leaf.size1, find_rank, &context, page, 0);
  if (context.fill == 0) return leaf.rank1 + leaf.size1;

  meta[oldest].rank1 = context.r;
  meta[oldest].size1 = context.fill;
  meta[oldest].age   = op;
  meta[oldest].lower = context.offsets[0];
  meta[oldest].upper = context.offsets[context.fill-1];

  return context.r;
}

struct pick_rank_context_t {
  rank_t r;
  offset_t *offsets;
};

static void pick_rank(void *_context, uint64_t rank, uint64_t value) {
  struct pick_rank_context_t *context = _context;
  if (rank >= context->r) {
    rank_t off = rank - context->r;
    if (off < DEPTH) {
      context->offsets[off] = value;
    }
  }
}

offset_t cache_select1(struct sbv_t *sbv, rank_t r) {
  struct meta_t *meta = &sbv->cache.ways[0];
  uint32_t op = ++sbv->cache.counter;
  uint32_t age = ~UINT32_C(0);
  int oldest;

  // counter overflow starts a new cache generation
  if (!op) cache_invalidate(sbv);

  for (int i = 0; i < WAYS; ++i) {
    rank_t off = r - meta[i].rank1;
    if (meta[i].rank1 <= r && off < meta[i].size1) {
      meta[i].age = op;
      return sbv->cache.offsets[i][off];
    }
    if (age >= meta[i].age) {
      oldest = i;
      age = meta[i].age;
    }
  }

  struct btree_result leaf = btree_select1(sbv, r);
  assert (leaf.rank1 <= r);
  if (r >= leaf.rank1 + leaf.size1) return 0;

  uint64_t *page = block_get(sbv, leaf.page);
  struct pick_rank_context_t context = { r, &sbv->cache.offsets[oldest][0] };

  decode(leaf.lower, leaf.upper, leaf.rank1, leaf.size1, pick_rank, &context, page, 0);
  rank_t size1 = leaf.rank1 + leaf.size1 - r;
  if (size1 > DEPTH) size1 = DEPTH;

  meta[oldest].rank1 = r;
  meta[oldest].size1 = size1;
  meta[oldest].age   = op;
  meta[oldest].lower = context.offsets[0];
  meta[oldest].upper = context.offsets[size1-1];

  return context.offsets[0];
}

void cache_insert(struct sbv_t *sbv, offset_t k) {
  cache_invalidate(sbv);
}

void cache_delete(struct sbv_t *sbv, offset_t k) {
  cache_invalidate(sbv);
}
