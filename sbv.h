#ifndef SBV_H
#define SBV_H

#include <stdint.h>

// A mutable succinct bit vector
typedef uint64_t offset_t;
typedef uint64_t rank_t;

struct sbv_t;

// pass 0 for an anonymous (not disk backed) bitvector
// returns 0 and sets errno if unable to open/map
struct sbv_t *sbv_open(const char *file);
void          sbv_close(struct sbv_t *sbv);

// These functions are all thread-safe
rank_t   sbv_rank1  (struct sbv_t *sbv, offset_t k); // # of set bits in the range [0, k)
rank_t   sbv_rank0  (struct sbv_t *sbv, offset_t k);
offset_t sbv_select1(struct sbv_t *sbv, rank_t r); // sequentially increasing r is much faster than decreasing
offset_t sbv_select0(struct sbv_t *sbv, rank_t r); // accesses beyond last element return 0

// Only one thread should use these at a time
int      sbv_test   (struct sbv_t *sbv, offset_t k);
void     sbv_set    (struct sbv_t *sbv, offset_t k);
void     sbv_unset  (struct sbv_t *sbv, offset_t k);

#endif
