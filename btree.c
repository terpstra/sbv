#include "sbv-imp.h"

#define C (BLOCK_SIZE/16)

#ifdef CLASSIC_BTREE
#define INSERT_NODE_SPLIT 1 // 50% utilization (1*100% => 2*50%); 100MB => 200MB
#define DELETE_NODE_MERGE (INSERT_NODE_SPLIT+1) // (2*50% => 1*100%)
#else
#define INSERT_NODE_SPLIT 3 // 75% utilization (3*100% => 4*75%); 100MB => 133MB
#define DELETE_NODE_MERGE (INSERT_NODE_SPLIT+1) // (4*75% => 3*100%)
#endif

struct node_t {
  uint64_t header; // fill, lock, whatever
  uint64_t keys[C-1];
  uint64_t payload[C]; // low 26b page#, high 38b size1
};

