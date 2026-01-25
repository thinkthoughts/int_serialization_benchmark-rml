#ifndef INTEGER_CONV_BASELINES_H
#define INTEGER_CONV_BASELINES_H

#include <array>
#include <immintrin.h>
#include <absl/strings/numbers.h>
#include "third_party/jeaiii_to_text.h"
#include "digitcount.h"
#include "digits.h"

// Declaration for the function we need are in
// third_party/itoa_an.c and third_party/itoa_yy.c
// Can't be in a namespace because of C linkage :(
extern "C" char *itoa_u64_an(uint64_t val, char *buf);
extern "C" char *itoa_u64_yy(uint64_t val, char *buf);

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

// FastIntToBuffer writes into result and returns pointer to '\0'
champagne_lemire_really_inline int absl_fastint(uint64_t value,
                                                char *const result) {
  const char* end = absl::numbers_internal::FastIntToBuffer(value, result);
  return int(end - result);
}

champagne_lemire_really_inline int jeaiii_fast_uint64(uint64_t v,
                                                      char *const result) {
  const char* end = jeaiii::to_text_from_integer(result, v);
  return int(end - result);
}

champagne_lemire_really_inline int itoa_an_64(uint64_t v, char *result) {
  const char* end = ::itoa_u64_an(v, result);
  return int(end - result);
}

champagne_lemire_really_inline int itoa_yy_64(uint64_t v, char *result) {
  const char* end = ::itoa_u64_yy(v, result);
  return int(end - result);
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

// Mathisen-style SIMD baseline for uint64_t
//
// DISCLAIMER:
//   This implementation is OUR INTERPRETATION of the "4 groups of 3 digits"
//   SSE4.1 variant described by StackOverflow user 'icecreamsword' in
//   https://stackoverflow.com/a/32818030/5183410.
//   It is NOT the original "TERJE_SSE" implementation:
//     - the source code is not publicly available;
//     - the version described only support 32-bit integers;
//
//   We:
//     - decompose uint64_t into base-1e9 chunks (3× up to 20 digits),
//     - print the most significant chunk with 1–9 digits (no leading zeros),
//     - print remaining chunks as fixed-width 9-digit blocks (zero-padded),
//     - inside each 32-bit < 1e9 chunk, split into three 3-digit groups,
//     - use SSE4.1 (pmulld + shifts) to extract hundreds/tens/ones in
//       three groups of 3 digits in parallel.

namespace mathisen_sse41_detail {

champagne_lemire_really_inline void groups3_to_ascii_sse41(
    uint32_t g2, uint32_t g1, uint32_t g0, char* dst) {
  // v = [g2, g1, g0, 0] in the 4 32-bits lanes
  __m128i v = _mm_setr_epi32(int(g2), int(g1), int(g0), 0);

  const __m128i mul41  = _mm_set1_epi32(41);
  const __m128i mul100 = _mm_set1_epi32(100);
  const __m128i mul103 = _mm_set1_epi32(103);
  const __m128i mul10  = _mm_set1_epi32(10);

  // q100 = v / 100, via (v * 41) >> 12  (valid for v < 1000)
  __m128i q100 = _mm_mullo_epi32(v, mul41);
  q100 = _mm_srli_epi32(q100, 12);

  // r100 = v - q100 * 100
  __m128i tmp100 = _mm_mullo_epi32(q100, mul100);
  __m128i r100   = _mm_sub_epi32(v, tmp100);

  // q10 = r100 / 10, via (r100 * 103) >> 10  (valid for r100 < 100)
  __m128i q10 = _mm_mullo_epi32(r100, mul103);
  q10 = _mm_srli_epi32(q10, 10);

  // ones = r100 - q10 * 10
  __m128i tmp10 = _mm_mullo_epi32(q10, mul10);
  __m128i ones  = _mm_sub_epi32(r100, tmp10);

  // hundreds = q100
  // tens     = q10
  // ones     = ones
  // All in [0,9]. On go back to scalar to assemble ASCII.
  alignas(16) uint32_t H[4];
  alignas(16) uint32_t T[4];
  alignas(16) uint32_t O[4];
  _mm_store_si128(reinterpret_cast<__m128i*>(H), q100);
  _mm_store_si128(reinterpret_cast<__m128i*>(T), q10);
  _mm_store_si128(reinterpret_cast<__m128i*>(O), ones);

  // ignore lane 3 (g=0), keep only g0,g1,g2
  for (int lane = 0; lane < 3; ++lane) {
    dst[3*lane + 0] = char('0' + H[lane]);
    dst[3*lane + 1] = char('0' + T[lane]);
    dst[3*lane + 2] = char('0' + O[lane]);
  }
}

// v < 1e9, write exactly 9-digits (zero-padded) in dst[0..8]
champagne_lemire_really_inline void u32_to_9digits_fixed_sse41(uint32_t v, char *const dst) {
  // Split in 3 groups of 3 digits:
  // v = g2 * 10^6 + g1 * 10^3 + g0
  uint32_t g2  = v / 1'000'000;
  uint32_t rem = v % 1'000'000;
  uint32_t g1  = rem / 1'000;
  uint32_t g0  = rem % 1'000;

  // Each gk is in [0,999], we get 3×3 digits, always
  // zero-padded, e.g., g2=5 → "005".
  groups3_to_ascii_sse41(g2, g1, g0, dst);
}

// v < 1e9, write 1–9 chiffres unpadded.
// return number of digits
champagne_lemire_really_inline int u32_to_1to9digits_scalar(uint32_t v, char *const dst) {
  // simple scalar for 1,2,3 digits
  if (v < 10) {
    dst[0] = char('0' + v);
    return 1;
  }
  if (v < 100) {
    uint32_t d1 = v / 10;
    uint32_t d0 = v % 10;
    dst[0] = char('0' + d1);
    dst[1] = char('0' + d0);
    return 2;
  }
  if (v < 1000) {
    uint32_t d2 = v / 100;
    uint32_t r  = v % 100;
    uint32_t d1 = r / 10;
    uint32_t d0 = r % 10;
    dst[0] = char('0' + d2);
    dst[1] = char('0' + d1);
    dst[2] = char('0' + d0);
    return 3;
  }

  // for 4–9 digits, we can reuse the SSE4.1 9-digits fixed function,
  // and strip leading zeros.
  char tmp[9];
  u32_to_9digits_fixed_sse41(v, tmp);

  int first = 0;
  while (first < 8 && tmp[first] == '0')
    ++first;

  int len = 9 - first;
  for (int i = 0; i < len; ++i)
    dst[i] = tmp[first + i];
  return len;
}

} // namespace mathisen_sse41_detail

champagne_lemire_really_inline int mathisen_sse(uint64_t value,
                                                char *const result) {
  using namespace mathisen_sse41_detail;

  // 1e9 per chunk
  // max uint64 = 18446744073709551615 (20 digits)
  // -> max 3 chunks.
  uint32_t chunks[3];
  int count = 0;

  while (value >= 1'000'000'000ULL) {
    chunks[count++] = static_cast<uint32_t>(value % 1'000'000'000ULL);
    value /= 1'000'000'000ULL;
  }
  chunks[count++] = static_cast<uint32_t>(value);

  // most-significant chunk: 1–9 digits without leading zeros.
  char* out = result;
  uint32_t ms = chunks[count - 1];
  int ms_digits = u32_to_1to9digits_scalar(ms, out);
  out += ms_digits;

  // remaining chunks: 9 digits, zero-padded.
  for (int i = count - 2; i >= 0; --i) {
    u32_to_9digits_fixed_sse41(chunks[i], out);
    out += 9;
  }

  return int(out - result);
}

// Mula's utoa64_sse function (Algorithm 1 in http://0x80.pl/notesen/2011-10-21-sse-itoa.html)
//
// DISCLAIMER:
//   - Mula's implementation (https://raw.githubusercontent.com/WojciechMula/toys/7731566/sse-utoa/sse64-intrin.c)
//     only supports up to 16-digit values;
//   - Therefore, we added a scalar path for prefix of values >= 1e16.
//   - Original returns a pointer to the middle of a static buffer (to skip leading zeros).
//   - Here, we copy to the output buffer after removing leading zeros, using the original's
//     O(1) movemask/ctz technique for leading-zero detection.

// Returns the 16-digit vector before ASCII conversion
champagne_lemire_really_inline __m128i mula_sse64_16digits_raw(uint64_t v) {
  // v must be < 1e16; returns digit values 0-9
  constexpr uint32_t DIV_10000 = 3518437209u;
  constexpr uint16_t DIV_100   = 5243u;      // floor(2^19 / 100)
  constexpr uint16_t DIV_10    = 52429u;     // floor(2^19 / 10)

  const __m128i div_10000 = _mm_set1_epi32(DIV_10000);
  const __m128i mul_10000 = _mm_set1_epi32(10000);
  const int     div_10000_shift = 45;

  const __m128i div_100   = _mm_set1_epi16(DIV_100);
  const __m128i mul_100   = _mm_set1_epi16(100);
  const int     div_100_shift = 3;

  const __m128i div_10  = _mm_set1_epi16(DIV_10);
  const __m128i mul_10  = _mm_set1_epi16(10);

  // Split into two 8-digit halves
  uint32_t a = uint32_t(v / 100000000);  // high 8 digits
  uint32_t b = uint32_t(v % 100000000);  // low  8 digits

  __m128i x = _mm_set_epi64x(b, a);

  // x div 10^4
  __m128i x_div_10000 = _mm_mul_epu32(x, div_10000);
  x_div_10000 = _mm_srli_epi64(x_div_10000, div_10000_shift);

  __m128i x_mod_10000 = _mm_mul_epu32(x_div_10000, mul_10000);
  x_mod_10000 = _mm_sub_epi32(x, x_mod_10000);

  // [mnop][ijkl][efgh][abcd]
  __m128i y = _mm_or_si128(x_div_10000, _mm_slli_epi64(x_mod_10000, 32));

  // y / 100
  __m128i y_div_100 = _mm_mulhi_epu16(y, div_100);
  y_div_100 = _mm_srli_epi16(y_div_100, div_100_shift);

  __m128i y_mod_100 = _mm_mullo_epi16(y_div_100, mul_100);
  y_mod_100 = _mm_sub_epi16(y, y_mod_100);

  // (AB,CD) pairs
  __m128i z = _mm_or_si128(y_div_100, _mm_slli_epi32(y_mod_100, 16));

  // z / 10
  __m128i z_div_10 = _mm_mulhi_epu16(z, div_10);
  z_div_10 = _mm_srli_epi16(z_div_10, 3);

  __m128i z_mod_10 = _mm_mullo_epi16(z_div_10, mul_10);
  z_mod_10 = _mm_sub_epi16(z, z_mod_10);

  return _mm_or_si128(z_div_10, _mm_slli_epi16(z_mod_10, 8));
}

champagne_lemire_really_inline int mula_sse64(uint64_t v, char *const result) {
  // 17–20 digits: 1–4 digits prefix + 16 digits suffix
  if (v >= 10000000000000000ULL) { // 1e16
    const uint64_t hi = v / 10000000000000000ULL;
    const uint64_t lo = v % 10000000000000000ULL;

    char* out = digits::write_one_two_three_or_four_digits_10000(result, hi);
    __m128i raw = mula_sse64_16digits_raw(lo);
    const __m128i ascii0 = _mm_set1_epi8('0');
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out), _mm_add_epi8(raw, ascii0));
    return int(out - result + 16);
  }

  if (v == 0) {
    result[0] = '0';
    return 1;
  }

  __m128i raw = mula_sse64_16digits_raw(v);

  // Leading-zero detection using movemask + ctz (from original Mula)
  // Compare each byte to zero; movemask gives a bit per byte; ctz finds first non-zero
  uint16_t mask = static_cast<uint16_t>(
      ~_mm_movemask_epi8(_mm_cmpeq_epi8(raw, _mm_setzero_si128())));
  int offset = __builtin_ctz(mask | 0x8000);

  // Convert to ASCII and store directly at result - offset
  const __m128i ascii0 = _mm_set1_epi8('0');
  __m128i ascii = _mm_add_epi8(raw, ascii0);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(result - offset), ascii);
  return 16 - offset;
}

} // namespace baselines_int

#endif
