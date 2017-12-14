#include <stdio.h>

static inline int index3(int x) {
  int x1 = x & 1;
  int x2 = x / 2;
  return (x1 == 1) ? 0 : 1*(x2+1);
}

static inline int index15(int x) {
  int x3 = x & 3;
  int x4 = x / 4;
  return (x3 == 3) ? index3(x4) : 3*(x4+1) + index3(x3);
}

static inline int index255(int x) {
  int x15 = x & 15;
  int x16 = x / 16;
  return (x15 == 15) ? index15(x16) : 15*(x16+1) + index15(x15);
}

#define LSB(i) ((i) & -(i))

int main() {
  // forward: sort order => tree position
  int forward[256], backward[256];
  forward[255] = 255;
  for (int i = 0; i < 255; ++i) forward[i] = index255(i);
  for (int i = 0; i <= 255; ++i) backward[forward[i]] = i;

  printf("  ");
  for (int i = 0; i <= 255; ++i) {
    printf("%3d, ", forward[i]);
    if (i % 16 == 15) printf("\n  ");
  }
  printf("\n\n  UDEF,");
  for (int i = 1; i < 255; ++i) {
    int idx = backward[i];
    int parent = idx + LSB(idx+1);
    // fprintf(stderr, "%d(%d) => %d(%d)\n", idx, i, parent, forward[parent]);
    printf("%3d, ", forward[parent]);
    if (i % 16 == 15) printf("\n  ");
  }
  printf("UDEF\n\n  ");
  for (int i = 1; i < 255; ++i) {
    int idx = backward[i];
    int parent = idx - LSB(idx);
    // fprintf(stderr, "%d(%d) => %d(%d)\n", idx, i, parent, forward[parent]);
    printf("%3d, ", forward[parent]);
    if (i % 16 == 15) printf("\n  ");
  }
  return 0;
}
