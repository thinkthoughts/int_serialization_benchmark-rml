#ifndef CHAMPAGNE_LEMIRE_H
#define CHAMPAGNE_LEMIRE_H

#include "portabilityutils.h"
#include "digits.h"
#include "digitcount.h"
#include "ifma_avx512.h"

#include <cstdlib>
#include <sys/types.h>

enum class Variant {
  Auto,          // Scan the data to decide the best variant
  Homogeneous,   // Optimized for Homogeneous digit lengths
  Heterogeneous  // Optimized for Heterogeneous digit lengths
};

#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512

// Small values: 0 .. 99'999'999  (1 to 8 digits)
template <Variant V>
champagne_lemire_really_inline int to_chars_small(uint64_t value, char *const result) {
    if constexpr (V == Variant::Homogeneous) {
        if (value < 10000) {
            return digits::write_one_two_three_or_four_digits_10000(result, value) - result;
        }
    }
    const __m512i digits_7_0 = to_string_avx512ifma_8digits(value);


    if constexpr (V == Variant::Homogeneous) {
        if (value >= 10000000) {  // exactly 8 digits
            _mm_storeu_si64(reinterpret_cast<__m128i*>(result), _mm512_cvtusepi64_epi8(digits_7_0));
            return 8;
        }
    }
    const uint32_t n = fast_digit_count(value);
    const __mmask8 mask = (__mmask8)(0xff00 >> n);
    _mm512_mask_cvtusepi64_storeu_epi8(result - 8 + n, mask, digits_7_0);
    return n;
}

// Medium values: 100'000'000 .. 9'999'999'999'999'999  (9 to 16 digits)
template <Variant V>
champagne_lemire_really_inline int to_chars_medium(uint64_t value, char *const result) {
    const __m128i digits_15_0 = to_string_avx512ifma(value);
    const uint32_t n = fast_digit_count(value);

    if constexpr (V == Variant::Homogeneous) {
        if (value >= 1000000000000000ULL) {  // exactly 16 digits
            _mm_storeu_si128(reinterpret_cast<__m128i*>(result), digits_15_0);
            return 16;
        }
    }

    const __mmask16 mask = (__mmask16)(0xFFFFu << (16 - n));
    _mm_mask_storeu_epi8(result - 16 + n, mask, digits_15_0);
    return n;
}

// Large values: 10'000'000'000'000'000 .. 18'446'744'073'709'551'615  (17–20 digits)
template <Variant V>
champagne_lemire_really_inline int to_chars_large(uint64_t value, char *const result) {
    if constexpr (V == Variant::Homogeneous) {
        const auto [q, r] = digits::div10e16(value);  // q = 1..1844, r < 10^16
        char *p = digits::write_one_two_three_or_four_digits_10000(result, q);
        const __m128i digits_15_0 = to_string_avx512ifma(r);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(p), digits_15_0);
        return static_cast<int>(p - result + 16);
    } else {  // Heterogeneous
        const uint32_t n = fast_digit_count(value);
        const auto [q, r] = digits::div10000<true>(value);
        const uint32_t nq = n - 4;
        const __m128i v16 = to_string_avx512ifma(q);
        const __mmask16 mask = (__mmask16)(0xFFFFu << (16 - nq));
        _mm_mask_storeu_epi8(result - 16 + nq, mask, v16);
        digits::write_four_digits_10000(result + nq, r);
        return n;
    }
}

// Dispatcher — the public / main entry point
template <Variant V>
champagne_lemire_really_inline int avx512_to_chars(uint64_t value, char *const result) {
    if (value < 100000000ULL) {
        return to_chars_small<V>(value, result);
    }

    if (value >= 10000000000000000ULL) {
        return to_chars_large<V>(value, result);
    }

    return to_chars_medium<V>(value, result);
}

#endif // CHAMPAGNE_LEMIRE_AVX512

#endif // CHAMPAGNE_LEMIRE_H
