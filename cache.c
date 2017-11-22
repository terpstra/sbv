#include "sbv-imp.h"

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

rank_t cache_rank1(struct sbv_t *sbv, offset_t k) {
  struct meta_t *meta = sbv->cache.ways[0];
  uint32_t op = ++sbv->cache.counter;
  uint32_t age = ~UINT32_C(0);
  int oldest;

  // counter overflow starts a new cache generation
  if (!op) cache_invalidate(sbv);

  for (int i = 0; i < WAYS; ++i) {
    if (meta[i].size1 && meta[i].lower <= k && k <= meta[i].upper) {
      meta[i].age = op;
      // search swizzled?
    }
    if (age >= meta[i].age) {
      oldest = i;
      age = meta[i].age;
    }
  }

  btree_rank1(sbv, k);
  // => lower, upper, rank1, size1, page#
  page = block_get(sbv, pagenr);
  return rank1 + leaf_rank1(lower, upper, page, size1, k);
}

offset_t cache_select1(struct sbv_t *sbv, rank_t r) {
  struct meta_t *meta = sbv->cache.ways[0];
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
  uint64_t *
  page = block_get(sbc, pagenr);
  // decode leaf into oldest cache entry
  leaf_select1(lower, upper, page, size1, r - rank1);
  // !!! do this part next, after defining b-tree methods, and generalizing decode

  return sbv->cache.offsets[oldest][r - meta[oldest].rank1];
}

void cache_insert(struct sbv_t *sbv, offset_t k) {
  cache_invalidate(sbv);
}

void cache_delete(struct sbv_t *sbv, offset_t k) {
  cache_invalidate(sbv);
}
