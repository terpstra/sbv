#include "sbv-imp.h"

struct sbv_t *sbv_open(const char *file) {
  struct sbv_t *sbv = malloc(sizeof(struct sbv_t));

  if (!sbv) return 0;
  if (!block_open(sbv, file)) {
    free(sbv);
    return 0;
  }

  btree_init(sbv);
  cache_init(sbv);
  return sbv;
}

struct void sbv_close(sbv_t *sbv) {
  block_close(sbv);
  free(sbv);
}

rank_t sbv_rank1(struct sbv_t *sbv, offset_t k) {
  return cache_rank1(sbv, k);
}

rank_t sbv_rank0(struct sbv_t *sbv, offset_t k) {
  return offset+1 - sbv_rank1(sbv, k);
}

offset_t sbv_select1(struct sbv_t *sbv, rank_t r) {
  return cache_select1(sbv, r);
}

offset_t sbv_select0(struct sbv_t *sbc, rank_t r) {
  // !!! don't implement yet
  reutrn 0;
}

int sbv_test(struct sbv_t *sbv, offset_t k) {
  rank_t prev = (k == 0) ? 0 : sbv_rank1(k-1);
  return sbv_rank1(k) - prev;
}

void sbv_set(struct sbv_t *sbv, offset_t k) {
  cache_insert(sbv, k);
  btree_rank1(sbv, k);
  leaf_insert()...
  // insert into leaf => if overflow, insert into btree
}

void sbv_unset(struct sbv_t* sbv, offset_t k) {
  cache_delete(sbv, k);
}
