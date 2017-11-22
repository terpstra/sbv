#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "encode.h"

#define SIZE 100000000

static int compare(const void *x, const void *y) {
 uint64_t a = *(uint64_t*)x;
 uint64_t b = *(uint64_t*)y;
 return -(a < b) + (a > b);
}

static void putter(void *context, uint64_t index, uint64_t value) {
  uint64_t *a = context;
  a[index] = value;
}

static uint64_t getter(void *context, uint64_t index) {
  uint64_t *a = context;
  return a[index];
}

int main(int argc, const char ** argv) {
  uint64_t *before = malloc(8*SIZE);
  uint64_t *stream = malloc(8*SIZE);
  uint64_t *after  = malloc(8*SIZE);
  uint64_t offset;
  struct timeval start, stop;
  uint64_t encode_us, decode_us;

  int n, i, s;
  for (n = 1; n <= SIZE; n += (n+3)/4) {
    for (i = 0; i < n; ++i)
      before[i] = (((uint64_t)random()) << 32 | random()) >> 1;

    qsort(before, n, 8, compare);
    memset(stream, 0, 8*SIZE);

    // prevent duplicates
    s = 0;
    for (i = 1; i < n; ++i) {
      before[i] += s;
      if (before[i-1] == before[i]) {
        ++before[i];
        ++s;
      }
      if (before[i-1] >= before[i]) printf("WTF: %llu %llu\n", before[i-1], before[i]);
      assert (before[i-1] < before[i]);
    }

    gettimeofday(&start, 0);
    offset = encode(0, ~UINT64_C(0), 0, n, getter, before, stream, 0);
    gettimeofday(&stop, 0);

    encode_us = ((uint64_t)stop .tv_sec * 1000000 + stop .tv_usec) -
                ((uint64_t)start.tv_sec * 1000000 + start.tv_usec);
    
    gettimeofday(&start, 0);
    decode(0, ~UINT64_C(0), 0, n, putter, after, stream, 0);
    gettimeofday(&stop, 0);

    decode_us = ((uint64_t)stop .tv_sec * 1000000 + stop .tv_usec) -
                ((uint64_t)start.tv_sec * 1000000 + start.tv_usec);

    printf("%d %llu %llu %llu\n", n, offset, encode_us, decode_us);
    fflush(stdout);

    for (i = 0; i < n; ++i) {
      // printf("%d %llu %llu\n", i, before[i], after[i]);
      assert (before[i] == after[i]);
    }
  }

  return 0;
}
