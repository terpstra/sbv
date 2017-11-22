#include "encode.h"
#include <assert.h>

// ceil(log2(x+1))
static int log2ceil_plus1(uint64_t x) {
 int clz;
 if (sizeof(unsigned int) == 8) {
   clz = __builtin_clz(x);
 } else if (sizeof(unsigned long) == 8) {
   clz = __builtin_clzl(x);
 } else if (sizeof(unsigned long long) == 8) {
   clz = __builtin_clzll(x);
 } else {
   assert (0);
 }
 return (64-clz) & -!!x;
}

static uint64_t encode_bits(
  uint64_t  value,
  int       bits,
  uint64_t *stream,
  uint64_t  offset)
{
  assert (0 <= bits && bits <= 64);
  uint64_t *base = stream + (offset >> 6);
  uint64_t low = offset & 63;
  uint64_t high = 64 - low;
  base[0] |= value << low;
  if (bits > high) base[1] = value >> high;
  return offset + bits;
}

static uint64_t decode_bits(
  uint64_t *value,
  int       bits,
  uint64_t *stream,
  uint64_t  offset)
{
  assert (0 <= bits && bits <= 64);
  uint64_t *base = stream + (offset >> 6);
  uint64_t low = offset & 63;
  uint64_t high = 64 - low;
  uint64_t out = base[0] >> low;
  if (bits > high) out |= base[1] << high;
  out &= (~UINT64_C(0) >> (64-bits))  & -!!bits;
  *value = out;
  return offset + bits;
}

uint64_t encode(
  uint64_t  lower,  // [lower, upper] inclusive range
  uint64_t  upper,
  uint64_t  data_i,
  uint64_t  data_n,
  get_t     getter, // must be sorted and strictly monotonic
  void     *context,
  uint64_t *stream,
  uint64_t  offset)
{
  assert (lower <= upper); // non-empty
  assert (0 < data_n && data_n-1 <= upper-lower); // well defined

  uint64_t left = data_n / 2;
  uint64_t right = data_n - (left+1);

  uint64_t min = lower + left;
  uint64_t max = upper - right;
  uint64_t value = getter(context, data_i + left);
  assert (min <= value && value <= max); // sorted and strictly monotonic

  offset = encode_bits(value-min, log2ceil_plus1(max-min), stream, offset);
  if (left)  offset = encode(lower, value-1, data_i,        left,  getter, context, stream, offset);
  if (right) offset = encode(value+1, upper, data_i+left+1, right, getter, context, stream, offset);
  return offset;
}

uint64_t decode(
  uint64_t  lower,  // [lower, upper] inclusive range
  uint64_t  upper,
  uint64_t  data_i,
  uint64_t  data_n,
  put_t     putter,
  void     *context,
  uint64_t *stream,
  uint64_t  offset)
{
  assert (lower <= upper); // non-empty
  assert (0 < data_n && data_n-1 <= upper-lower); // well defined

  uint64_t left = data_n / 2;
  uint64_t right = data_n - (left+1);

  uint64_t min = lower + left;
  uint64_t max = upper - right;
  uint64_t value;

  offset = decode_bits(&value, log2ceil_plus1(max-min), stream, offset);
  value += min;
  putter(context, data_i+left, value);
  assert (min <= value && value <= max); // sorted and strictly monotonic

  if (left)  offset = decode(lower, value-1, data_i,        left,  putter, context, stream, offset);
  if (right) offset = decode(value+1, upper, data_i+left+1, right, putter, context, stream, offset);
  return offset;
}
