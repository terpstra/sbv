#ifndef BLOCK_H
#define BLOCK_H

#include "sbv.h"

#define BLOCK_SIZE 256

struct block_t {
  int fd;
  void *map;
  uint64_t size;
};

typedef uint32_t page_t;

void *block_get(struct sbv_t *sbv, page_t pagenr);
int block_open(struct sbv_t *sbv, const char *file);
void block_close(struct sbv_t *sbv);

// Interface to the backing store:
/*
typedef uint64_t block_t;
void *bse_acquire(block_t); // Obtain pointer memory-mapped view of the block
void bse_release(block_t);  // Release memory-mapped view of the block (counted)
void bse_usage(block_t);    // Set the number of blocks required (shrink invalidates open acquire?)
*/


#endif
