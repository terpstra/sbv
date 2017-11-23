#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>

typedef void (*put_t)(void* context, uint64_t index, uint64_t value);
typedef uint64_t (*get_t)(void *context, uint64_t index);

uint64_t encode(
  uint64_t  lower,  // [lower, upper] inclusive range
  uint64_t  upper,
  uint64_t  data_i,
  uint64_t  data_n,
  get_t     getter, // must be sorted and strictly monotonic; called in tree pre-order
  void     *context,
  uint64_t *stream,
  uint64_t  offset);

uint64_t decode(
  uint64_t  lower,  // [lower, upper] inclusive range
  uint64_t  upper,
  uint64_t  data_i,
  uint64_t  data_n,
  put_t     putter, // called with output in-order
  void     *context,
  uint64_t *stream,
  uint64_t  offset);

#endif
