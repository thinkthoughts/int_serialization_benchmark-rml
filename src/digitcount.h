#ifndef DIGITCOUNT_H
#define DIGITCOUNT_H

#include <bit>
#include <cstdint>
#include "portabilityutils.h"

champagne_lemire_really_inline int int_log2_64(uint64_t x) {
  return 63 - std::countl_zero(x | 1);
}

/**
 * Reference:
 * Daniel Lemire, "Computing the number of digits of an integer even faster,"
 * in Daniel Lemire's blog, June 3, 2021,
 * https://lemire.me/blog/2021/06/03/computing-the-number-of-digits-of-an-integer-even-faster/.
 */
champagne_lemire_really_inline int fast_digit_count32(uint32_t x) {
  static uint64_t table[] = {
    4294967296,  8589934582,  8589934582,  8589934582,  12884901788,
    12884901788, 12884901788, 17179868184, 17179868184, 17179868184,
    21474826480, 21474826480, 21474826480, 21474826480, 25769703776,
    25769703776, 25769703776, 30063771072, 30063771072, 30063771072,
    34349738368, 34349738368, 34349738368, 34349738368, 38554705664,
    38554705664, 38554705664, 41949672960, 41949672960, 41949672960,
    42949672960, 42949672960};
  return uint32_t((x + table[int_log2_64(x)]) >> 32);
}

/**
 * Reference:
 * Daniel Lemire, "Counting the digits of 64-bit integers,"
 * in Daniel Lemire's blog, January 7, 2025,
 * https://lemire.me/blog/2025/01/07/counting-the-digits-of-64-bit-integers/.
 */
champagne_lemire_really_inline int fast_digit_count64(uint64_t x) {
  static int digits[65] = {19, 19, 19, 19, 18, 18, 18,
                           17, 17, 17, 16, 16, 16,
                           16, 15, 15, 15, 14, 14, 14,
                           13, 13, 13, 13, 12, 12,
                           12, 11, 11, 11, 10, 10, 10,
                           10, 9,  9,  9,  8,  8,
                           8,  7,  7,  7,  7,  6,  6,
                           6,  5,  5,  5,  4,  4,
                           4,  4,  3,  3,  3,  2,  2,
                           2,  1,  1,  1,  1,  1};
  static uint64_t table[65] = {9999999999999999999ULL,
                               9999999999999999999ULL,
                               9999999999999999999ULL,
                               9999999999999999999ULL,
                               999999999999999999ULL,
                               999999999999999999ULL,
                               999999999999999999ULL,
                               99999999999999999ULL,
                               99999999999999999ULL,
                               99999999999999999ULL,
                               9999999999999999ULL,
                               9999999999999999ULL,
                               9999999999999999ULL,
                               9999999999999999ULL,
                               999999999999999ULL,
                               999999999999999ULL,
                               999999999999999ULL,
                               99999999999999ULL,
                               99999999999999ULL,
                               99999999999999ULL,
                               9999999999999ULL,
                               9999999999999ULL,
                               9999999999999ULL,
                               9999999999999ULL,
                               999999999999ULL,
                               999999999999ULL,
                               999999999999ULL,
                               99999999999ULL,
                               99999999999ULL,
                               99999999999ULL,
                               9999999999ULL,
                               9999999999ULL,
                               9999999999ULL,
                               9999999999ULL,
                               999999999ULL,
                               999999999ULL,
                               999999999ULL,
                               99999999ULL,
                               99999999ULL,
                               99999999ULL,
                               9999999ULL,
                               9999999ULL,
                               9999999ULL,
                               9999999ULL,
                               999999ULL,
                               999999ULL,
                               999999ULL,
                               99999ULL,
                               99999ULL,
                               99999ULL,
                               9999ULL,
                               9999ULL,
                               9999ULL,
                               9999ULL,
                               999ULL,
                               999ULL,
                               999ULL,
                               99ULL,
                               99ULL,
                               99ULL,
                               9ULL,
                               9ULL,
                               9ULL,
                               9ULL,
                               0ULL};
  int log = std::countl_zero(x);
  uint64_t low = table[log];
  uint64_t high = digits[log];
  return (x > low) + high;
}

template <typename T>
champagne_lemire_really_inline int fast_digit_count(T x) {
  if constexpr (sizeof(T) == 4) {
    return fast_digit_count32(x);
  } else if constexpr (sizeof(T) == 8) {
    return fast_digit_count64(x);
  } else {
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported type size");
  }
}

#endif
