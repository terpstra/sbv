#ifndef BTREE_H
#define BTREE_H

#include "block.h"

struct btree_t {
  page_t root;
};

struct btree_result {
  offset_t lower, upper;
  rank_t rank1;
  uint32_t size1;
  page_t page;
};

void btree_init(struct sbv_t* sbv);

// find leaf where lower <= k <= upper
struct btree_result btree_rank1(struct sbv_t *sbv, offset_t k);

// find leaf where rank1 <= r < rank1+size1 || last page
struct btree_result btree_select1(struct sbv_t *sbv, rank_t r);

#endif
