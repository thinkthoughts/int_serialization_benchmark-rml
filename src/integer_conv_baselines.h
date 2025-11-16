#ifndef INTEGER_CONV_BASELINES_H
#define INTEGER_CONV_BASELINES_H

#include <array>
#include "digitcount.h"

namespace baselines_int {

champagne_lemire_really_inline int naive(uint64_t value, char *const result) {
  const int number_of_digits = fast_digit_count(value);
  uint32_t i = number_of_digits;
  while(i--) {
    const uint64_t digit = value % 10;
    value /= 10;
    result[i] = '0' + digit;
  }
  return number_of_digits;
}

// Implementation of hopman_fast from
// https://stackoverflow.com/a/4364057/5183410
// I Adapted to support uint64_t and use a buffer pointer, returning the length.

constexpr auto hopman_table = []() {
  std::array<std::array<char, 4>, 10000> table{};
  for (int i = 0; i < 10000; ++i) {
    int v = i;

    char d3 = char('0' + (v      % 10));
    char d2 = char('0' + (v/10   % 10));
    char d1 = char('0' + (v/100  % 10));
    char d0_raw = char(v / 1000);  // 0..9

    char mask;
    if (d0_raw != 0) mask = char(d0_raw | 0x30);
    else if (d1 != '0') mask = char(0x20);
    else if (d2 != '0') mask = char(0x10);
    else mask = char(0x00);

    table[i] = { mask, d1, d2, d3 };
  }
  return table;
}();

champagne_lemire_really_inline int hopman_fast(uint64_t value, char *const result) {
  // Special case: zero
  if (value == 0) {
    result[0] = '0';
    return 1;
  }

  // Split into 5 blocks of 4 decimal digits (base 10000)
  // max uint64 = 18446744073709551615 (20 digits) → fits 5 blocks
  uint32_t b[5];
  b[4] = uint32_t(value % 10000); value /= 10000;
  b[3] = uint32_t(value % 10000); value /= 10000;
  b[2] = uint32_t(value % 10000); value /= 10000;
  b[1] = uint32_t(value % 10000); value /= 10000;
  b[0] = uint32_t(value);         // ≤ 1844

  // Find first non-zero block
  int first = 0;
  while (first < 5 && b[first] == 0)
    first++;

  char* out = result;

  {
    const char* c = hopman_table[b[first]].data();
    const unsigned mask = uint8_t(c[0]) >> 4; // 0..3
    const int skip = 3 - int(mask);           // index of first real digit

    for (int j = skip; j < 4; ++j) {
      *out++ = c[j];
    }
  }

  for (int i = first + 1; i < 5; ++i) {
    const char* c = hopman_table[b[i]].data();

    // Hopman original: OR entire 32-bit block with 0x30303030
    // Here: per-byte equivalent
    *out++ = char(uint8_t(c[0]) | 0x30);
    *out++ = c[1];
    *out++ = c[2];
    *out++ = c[3];
  }

  return int(out - result);
}

} // namespace baselines_int

#endif
