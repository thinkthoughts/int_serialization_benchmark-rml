#ifndef IFMA_AVX512_H
#define IFMA_AVX512_H

/**
 * Daniel Lemire, "Converting integers to decimal strings faster with AVX-512,"
 * in Daniel Lemire's blog, March 28, 2022,
 * https://lemire.me/blog/2022/03/28/converting-integers-to-decimal-strings-faster-with-avx-512/.
 */

#if defined(__AVX512IFMA__) && defined(__AVX512VBMI__)
#define CHAMPAGNE_LEMIRE_AVX512 1
#include "portabilityutils.h"
#include <cstdint>
#include <x86intrin.h>

/*
The IFMA decimal print method:

Computing remainder of n by 10, 100, ..., 10^8 and then multiplication by 10.

n = 84736251
1              = n mod 10
51             = n mod 100
251            = n mod 1000
6251
36251
736251
4736251
84736251       = n mod 10^8

From this paper

Lemire, D., Bartlett, C., & Kaser, O. (2021). Integer division by constants: optimal bounds. Heliyon, 7(6).
https://arxiv.org/abs/2012.12369

Theorem 4 (page 3)

It says that ( (c * n + c) % m ) * d / m gives n mod d, for 0 <= n <= N as long as 

(1 - 1/(N+1))*1/d ≤ c/m  < 1/d

or

N m ≤c d (N+1) < m (N+1)


As long as d does not divide m, we can set c = floor (m / d) and c/m  < 1/d is satisfied. 

It remains to verify the left identity.

N m ≤c d (N+1)

We want m to be 2^52 and N = 10^8 - 1, so we need to verify that

(10^8 - 1) * 2^52 ≤ floor(2^52 / d) * d * 10^8

where d = 10, ... , 10^8 

In Python, we can check:

for k in range(1,9):
  d = 10**k
  lhs = (10**8-1) * 2**52
  rhs = (2**52//d)*d*10**8
  assert lhs <= rhs

This fits well with the IFMA instruction pair, which computes (c * n + c) mod 2^52 and then multiplies the result by 10 and adds '0' to get the ASCII code of the digit.

We set call 'c' ifma_const, set m = 2^52, and we compute

(c  n + c) % 2^52 as

lowbits_l = _mm512_madd52lo_epu64(ifma_const, bcstq_l, ifma_const)

and then we compute

((c  n + c) % 2^52) * 10 + '0' as

_mm512_madd52hi_epu64(asciiZero, zmmTen, lowbits_l)

where asciiZero is the vector of '0' characters and zmmTen is the vector of 10s.



*/


champagne_lemire_really_inline __m512i to_string_avx512ifma_8digits(uint64_t n) {
  __m512i bcstq_l   = _mm512_set1_epi64(n);
  constexpr uint64_t twoto52 = 0x10000000000000ULL; // 2^52
  __m512i ifma_const = _mm512_setr_epi64(
    twoto52 / 100000000, twoto52 / 10000000, twoto52 / 1000000, twoto52 / 100000,
    twoto52 / 10000, twoto52 / 1000, twoto52 / 100, twoto52 / 10
  );
  __m512i zmmzero   = _mm512_castsi128_si512(_mm_cvtsi64_si128(0x01A1A400));
  __m512i zmmTen    = _mm512_set1_epi64(10);
  __m512i asciiZero = _mm512_set1_epi64('0');
  __m512i lowbits_l  = _mm512_madd52lo_epu64(ifma_const, bcstq_l, ifma_const); // ifma_const * bcstq_l + ifma_const
  __m512i highbits_l = _mm512_madd52hi_epu64(asciiZero, zmmTen, lowbits_l);
  return highbits_l;
}

// caller is responsible for providing a buffer of at least 16 bytes
// the output is not null-terminated
// the output is 16 bytes long
// The value n should be no larger than 9999999999999999
// That is, it needs to be in [0, 10^16)
champagne_lemire_really_inline __m128i to_string_avx512ifma(uint64_t n) {
  uint64_t n_15_08  = n / 100000000;
  uint64_t n_07_00  = n % 100000000;
  __m512i bcstq_h   = _mm512_set1_epi64(n_15_08);
  __m512i bcstq_l   = _mm512_set1_epi64(n_07_00);
  constexpr uint64_t twoto52 = 0x10000000000000ULL; // 2^52
  __m512i ifma_const = _mm512_setr_epi64(
    twoto52 / 100000000, twoto52 / 10000000, twoto52 / 1000000, twoto52 / 100000,
    twoto52 / 10000, twoto52 / 1000, twoto52 / 100, twoto52 / 10
  );

  __m512i zmmTen    = _mm512_set1_epi64(10);
  __m512i asciiZero = _mm512_set1_epi64('0');

  __m512i permb_const = _mm512_castsi128_si512(
      _mm_set_epi8(0x78, 0x70, 0x68, 0x60, 0x58, 0x50, 0x48, 0x40,
                   0x38, 0x30, 0x28, 0x20, 0x18, 0x10, 0x08, 0x00));
  __m512i lowbits_h	  = _mm512_madd52lo_epu64(ifma_const, bcstq_h, ifma_const); // lowbits_h = ifma_const * bcstq_h + ifma_const
  __m512i lowbits_l	  = _mm512_madd52lo_epu64(ifma_const, bcstq_l, ifma_const); // lowbits_l = ifma_const * bcstq_l + ifma_const
  __m512i highbits_h	= _mm512_madd52hi_epu64(asciiZero, zmmTen, lowbits_h); // highbits_h = lowbits_h * 10 + asciiZero
  __m512i highbits_l	= _mm512_madd52hi_epu64(asciiZero, zmmTen, lowbits_l); // highbits_l = lowbits_l * 10 + asciiZero

  // idx & 0x40 ? highbits_h[idx & 0x3F] : highbits_l[idx & 0x3F]
  __m512i perm        = _mm512_permutex2var_epi8(highbits_h, permb_const, highbits_l);
  __m128i digits_15_0	= _mm512_castsi512_si128(perm);
  return digits_15_0;
}

#endif // defined(__AVX512IFMA__) && defined(__AVX512VBMI__)

#endif // IFMA_AVX512_H
