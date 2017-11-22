#ifndef CACHE_H
#define CACHE_H

#include "sbv.h"

#define WAYS	4
#define DEPTH	128

struct meta_t {
  rank_t rank1;
  uint32_t size1; // size1 = 0 => invalid
  uint32_t age;   // to pick replacement victim
  offset_t lower, upper;
};

struct cache_t {
  struct meta_t ways[WAYS];
  uint32_t counter;
  offset_t offsets[WAYS][DEPTH]; // includes lower/upper
};

void     cache_init   (struct sbv_t *sbv);
rank_t   cache_rank1  (struct sbv_t *sbv, offset_t k);
offset_t cache_select1(struct sbv_t *sbv, rank_t r);
void     cache_insert (struct sbv_t *sbv, offset_t k);
void     cache_delete (struct sbv_t *sbv, offset_t k);

#endif
