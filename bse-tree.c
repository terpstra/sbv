#include "bse.h"

/* Format of a BSE internal node:
 *   weight (1 bit)
 *   replaced (1 bit)
 *   key [0, n-1)
 *   ptr [0, n)
 */
#define BSE_INTERIOR_CHILDREN (BSE_BLOCK_SIZE / (sizeof(offset_t) + sizeof(block_t)))

/* Format of a BSE leaf node:
 *   uint64_t first, last;

 Minimum number per leaf 32 * 8 byte

 5*32 highest bits encoded into 64 bit
 */
#define BSE_LEAF_CHILDREN (


// All more than half full => no need to 
// 128 => 0

// Start search not at middle, but at power-of-two middle
// 64, (32, 96), (16, 48, 

// 64-bit child
// 64-bit rank
// 


// 


// (0 1 2) (3 7 8) (4 9 10) (5 11 12) (6 13 14) (


/*
                                               32
                       16                                              48
            8                      24                      40                      56
      4          12          20          28          36          44          52          60
   2     6    10    14    18    22    26    30    34    38    42    46    50    54    58    62
 1  3  5  7  9 11 13 15 17 19 21 23 25 27 29 31 33 35 37 39 41 43 45 47 49 51 53 55 57 59 61 63

                                                0
                        1                                               2
            3                       6                       9                      12
      4           5           7           8          10          11          13          14




  15    18    21    24    27    30    33    36    39    42    45    48    51    54    57    60

16 17 19 20 22 23 25 26 28 29 31 32 34 35 37 38 40 41 43 44 46 47 49 50 52 53 55 56 58 59 61 62



cache block-sized key clusters
NO POINTERS -- use complete tree
children derived directly from arithmetic
... what if interior nodes have holes? => exponential space wastage

10% slack at root => 10% loss, 10% slack at level 2 => 9% loss, ... 1 - 0.9^levels
if key and child pointer are similar size ... complete tree savings is worthwhile!
... possible to avoid waste?

14,15,16 / 16 allowed

when full, ask parent to rebalance with 14 other nodes (urp -- parent needs to be sure there are 15?!)
when all are full (16*15) => become 15/16 for all 15 off us => creating a new 15/16 node => insert this node in parent (recursively)
when low, ask parent to rebalance with 14 other nodes
when all are low (14*15) => become 15/16 for 14 of us => delete wasted node in parent (recursively)

0-10% waste allowed
when full, ask parent to rebalance with +18 other nodes
all full => become 20 95% full nodes
when low, ask parent to rebalance with +18 other nodes
all low => become 18 95% full nodes

NOTE: the parent cannot always rebalance with all the nodes needed
=> just do best effort; e.g. split 100% * 4 => 5 * 80%
... the tolerable low mark depends on the number of nodes available! (for 5 it is 60%)

to achieve 80%-100% you need to balance over +(10-2) nodes (9 including target)
to achieve 60%-100% you need to balance over +(5-2) nodes (4 including target)
to achieve 50%-100% you need to balance over +(4-2) nodes (3 including target)
... not the same as 2/3/4 nodes; always reverts to center

uncenetered version is assymetric; need to consider 1 more node when deleting than adding
to achieve 90%-100% you need to balance over +(10-2) nodes (9 including target, or 10 for delete)
to achieve 80%-100% you need to balance over +(5-2) nodes (4 including target, or 5 for delete)
to achieve 75%-100% you need to balance over +(4-2) nodes (3 including target, or 4 for delete)
to achieve 50%-100% you need to balance over +(2-2) nodes (1 including target, or 2 for delete)

(keys, order) => child ... do keys in nodes appear again in leaves? if no: allows leaves to be fully Elias deltas
... when you reach a leaf you have lower and upper already (0, 2^64 for primodial node) => bounds space and defines encoding
=> also means parents know capacity of children ... combined with order statistic => parents also know space available
  ... handy b/c otherwise need to scan elias bitmap

block format open(order, [low, high])

8-byte header
255 * 8-byte keys; 17*15; 5*3
rest bits: 38 order and 26 block (max 256GE or 258GB, whichever comes first)


Worst-case?
  50%

Best-case?
  64 + 2*63 + 4*62 + 8*61 + 16*60 + 2^64*0
  
  Sum_{i=0..64} 2^i * (64-i)

*/
