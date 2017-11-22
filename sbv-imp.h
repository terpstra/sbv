#ifndef SBV_IMP_H
#define SBV_IMP_H

#include "block.h"
#include "cache.h"
#include "btree.h"

struct sbv_t {
  struct block_t block;
  struct btree_t btree;
  struct cache_t cache;
};

#endif
