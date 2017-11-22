#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define RANGE 65536

static const int log2Floor(uint32_t v) {
 static const int MultiplyDeBruijnBitPosition2[32] = {
    0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
    8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
  };

  v |= v >> 1; // first round down to one less than a power of 2 
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;

  return MultiplyDeBruijnBitPosition2[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}

static const int log2Ceil(uint32_t v) {
  if (v == 1) return 0;
  return log2Floor(v-1) + 1;
}

static uint64_t lowerBound(int r, int k) {
  // ceil(log_2 (r choose k))
  double lchoose = lgamma(r+1) - lgamma(k+1) - lgamma((r-k)+1); // r!/[k!(r-k)!]
  double lchoose2 = lchoose / log(2);
  return ceil(lchoose2-0.01);
}

// cost[KEYS][RANGE]; KEYS > RANGE is undefined
uint64_t cost[RANGE+1][RANGE+1];

int main() {
  int k, r;
  // for (int i = 1; i < 55; ++i) { printf("%d => %d %d\n", i, log2Floor(i), log2Ceil(i)); }
  for (r = 2; r <= RANGE; ++r) {
    double worst_r = 0;
    int worst_k;
    for (k = 1; k <= r-1; ++k) {
      // what does the best possible encoding cost?
      uint64_t limit = lowerBound(r, k);
      double ratio;
      // flip encoding to negative
      if (k > r/2) {
        cost[k][r] = cost[r-k][r];
      } else {
        // the subtree divides keys in this way:
        int k0 = k / 2;
        int k1 = (k-1) - k0;
        int r0, r1;
        uint64_t worst = 0;
        assert (k0 >= 0 && k1 >= 0);
        // Walk r0 through all the possible values for the median
        assert (k0 <= (r-1)-k1);
        for (r0 = k0; r0 <= (r-1)-k1; ++r0) {
          r1 = (r-1) - r0;
          assert (k0 <= r0 && k1 <= r1);
          uint64_t trial = cost[k0][r0] + cost[k1][r1];
          if (trial > worst) worst = trial;
        }
        // truncated binary code
        cost[k][r] = worst + log2Ceil(r-(k-1));
        // arithmetic code
        // cost[k][r] = worst + ceil(log2(r-(k-1))*1000);
      }

      // Keep score:
      assert (cost[k][r] >= limit);
      ratio = cost[k][r]*1.0 / limit;
      if (ratio > worst_r) {
        worst_k = k;
        worst_r = ratio;
      }
    }
   printf("%d %d %f %llu %llu %f\n", worst_k, r, 1.0*cost[worst_k][r]/worst_k, cost[worst_k][r], lowerBound(r, worst_k), 1.0*cost[worst_k][r]/lowerBound(r, worst_k));
  }
  return 0;
}
