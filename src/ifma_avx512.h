/**
 * Daniel Lemire, "Converting integers to decimal strings faster with AVX-512," in Daniel Lemire's blog, March 28, 2022, https://lemire.me/blog/2022/03/28/converting-integers-to-decimal-strings-faster-with-avx-512/.
 */



#if defined(__AVX512IFMA__) && defined(__AVX512VBMI__) 

#define CHAMPAGNE_LEMIRE_AVX512 1


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
https://arxiv.org/abs/1902.01961
 page 8:

      uint32_t d = ...; // your divisor > 0
      // c = ceil ( (1 < <64) / d ) ; we take L = N
      uint64_t c = UINT64_C (0 xFFFFFFFFFFFFFFFF ) / d + 1;
      // fastmod computes (n mod d) given precomputed c
      uint32_t fastmod ( uint32_t n, uint64_t c, uint32_t d) {
          uint64_t lowbits = c * n;
          return (( __uint128_t ) lowbits * d) >> 64;
      }

Fastmod fits well for this AVX512FMA instruction pair: 
VPMADD52LUQ => lowbits = c * n + 0
VPMADD52HUQ => highbits = lowbits * 10 + asciiZero
just uses 52b and 104b numbers instead of 64 and 128, and highbits use 10 instead of d, and produces 8 decimal digits for 0 <= n <= 99999999.

The only problem is that in the 8th digit case the VPMADD52HUQ overflows, if we use the orginial 0x2af31dd ( = (2^53 - 1)/(10^8) + 1) constant as c in VPMADD52LUQ:

0x2af31dd * 99999999 = 0x10000001a50b23 

Solution: we use 0x2af31dc = 0x2af31dd - 1 as c, and use 0x1A1A400 bias instead of 0. 0x1A1A400 is the smallest bias, which does not underflows in case of the smallest 8-digit number:

0x2af31dc * 10000000 = 0x19999996FD600 = 450359960000000
(0x19999996FD600 + 0x1A1A400) * 10 = 0x1000000EAEC400
*/ 

// caller is responsible for providing a buffer of at least 16 bytes
// the output is not null-terminated
// the output is 16 bytes long
// The value n should be no larger than 9999999999999999
// That is, it needs to be in [0, 10^16)
void to_string_avx512ifma(uint64_t n, char *out) {
  uint64_t n_15_08  = n / 100000000;
  uint64_t n_07_00  = n % 100000000;
  __m512i bcstq_h   = _mm512_set1_epi64(n_15_08);
  __m512i bcstq_l   = _mm512_set1_epi64(n_07_00);
  __m512i zmmzero   = _mm512_castsi128_si512(_mm_cvtsi64_si128(0x1A1A400));
  __m512i zmmTen    = _mm512_set1_epi64(10);
  __m512i asciiZero = _mm512_set1_epi64('0');

  __m512i ifma_const	= _mm512_setr_epi64(0x00000000002af31dc, 0x0000000001ad7f29b, 
    0x0000000010c6f7a0c, 0x00000000a7c5ac472, 0x000000068db8bac72, 0x0000004189374bc6b,
    0x0000028f5c28f5c29, 0x0000199999999999a);
  __m512i permb_const	= _mm512_castsi128_si512(_mm_set_epi8(0x78, 0x70, 0x68, 0x60, 0x58,
    0x50, 0x48, 0x40, 0x38, 0x30, 0x28, 0x20, 0x18, 0x10, 0x08, 0x00));
  __m512i lowbits_h	= _mm512_madd52lo_epu64(zmmzero, bcstq_h, ifma_const);
  __m512i lowbits_l	= _mm512_madd52lo_epu64(zmmzero, bcstq_l, ifma_const);
  __m512i highbits_h	= _mm512_madd52hi_epu64(asciiZero, zmmTen, lowbits_h);
  __m512i highbits_l	= _mm512_madd52hi_epu64(asciiZero, zmmTen, lowbits_l);
  __m512i perm          = _mm512_permutex2var_epi8(highbits_h, permb_const, highbits_l);
  __m128i digits_15_0	= _mm512_castsi512_si128(perm);
  _mm_storeu_si128((__m128i *)out, digits_15_0);
}

#endif