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
template <typename T>
champagne_lemire_really_inline int avx512_to_chars(T mantissa, int32_t exponent, char *const result) {
  static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported type size");

  int32_t exp = exponent;
  size_t exp_index;
  if (mantissa >= 10'000'000'000'000'000) {
    // The special case where we max out the number of
    // digits exceeds 16 digits, and we handle it separately.
    // The mantissa is in [10^16, 10^17)
    // Ok, so we have to write 17 digits.
    //
    // We need to write a digit, a dot and 16 digits.
    uint64_t top_digit = mantissa / 10'000'000'000'000'000;
    digits::write_one_digit_with_dot(result, top_digit);

    // The call to to_string_avx512ifma and its storage amount to about
    // 25 instructions, and that can be about a third of the processing time.
    __m128i digits_15_0 = to_string_avx512ifma(mantissa % 10'000'000'000'000'000);
    _mm_storeu_si128((__m128i *)(result + 2), digits_15_0);
    exp += 16;
    exp_index = 18; // 17 digits + dot
  } else if ( mantissa >= 1'000'000'000'000'000) {
    // We need to write a digit, a dot and 15 digits.
    uint64_t top_digit = mantissa / 1'000'000'000'000'000;
    digits::write_one_digit_with_dot(result, top_digit);
    // The call to to_string_avx512ifma and its storage amount to about
    // 25 instructions, and that can be about a third of the processing time.
    __m128i digits_15_0 = to_string_avx512ifma(mantissa % 1'000'000'000'000'000);
    _mm_mask_storeu_epi8((__m128i *)(result + 1), 0xfffe, digits_15_0);
    exp += 15;
    exp_index = 17; // 16 digits + dot
  } else {
    __m128i digits_15_0 = mantissa <= 99'999'999 ?
        to_string_avx512ifma_8digits(mantissa) :
        to_string_avx512ifma(mantissa);

    const uint32_t number_of_digits = fast_digit_count(mantissa);
    exp += number_of_digits - 1;
    bool use_dot = (mantissa >= 10);
    exp_index = number_of_digits + use_dot;
    digits_15_0 = shift_and_insert_dot(digits_15_0, number_of_digits);
    _mm_mask_storeu_epi8(result, (1 << (number_of_digits + use_dot)) - 1, digits_15_0);
  }

  // Finally, we may have to handle the exponent.
  //
  // It looks simple but it is extraordinarily expensive relatively speaking.
  //
  // This may account for a THIRD for the processing in terms of instructions.
  // Maybe 25 instructions?
  if (mantissa && exp) { // We do not print the exponent if mantissa is zero but zero is handled above.
    // About 20 instructions for the exponent?
    memcpy(result + exp_index, "E-", 2);
    exp_index += 1 + (exp < 0);
    exp = (exp < 0) ? -exp : exp;

    if constexpr (sizeof(T) == 8) {
      if (exp >= 100) { // 3 digits
        digits::write_three_digits(result + exp_index, exp);
        exp_index += 3;
      } else { // 2 digits
        // If we need fewer than 2 digits, this will write a leading zero.
        digits::write_two_digits(result + exp_index, exp);
        exp_index += 2;
      }
    } else {
      // If we need fewer than 2 digits, this will write a leading zero.
      digits::write_two_digits(result + exp_index, exp);
      exp_index += 2;
    }
  }

  return exp_index;
}

template <Variant V>
int avx512_to_chars(uint64_t value, char *const result) {
  if(value < 100000000) { // 10^8
    if constexpr (V == Variant::Homogeneous) {
      if (value < 10000) // 10^4
        return digits::write_one_two_three_or_four_digits_10000(result, value) - result;
    }
    const __m128i digits_7_0 = to_string_avx512ifma_8digits(value);
    const uint32_t n = fast_digit_count(value);
    if constexpr (V == Variant::Homogeneous) {
      if (value >= 10000000) { // number has 8 digits
        _mm_storeu_si64(reinterpret_cast<__m128i*>(result), digits_7_0);
        return 8;
      }
    }
    const __mmask16 mask = (__mmask16)(0xFFFFu << (16 - n));
    _mm_mask_storeu_epi8(result - 16 + n, mask, digits_7_0);
    return n;
  }

  const uint32_t n = fast_digit_count(value);
  if (value < 10000000000000000ULL) { // 10^16
    const __m128i digits_15_0 = to_string_avx512ifma(value);
    if constexpr (V == Variant::Homogeneous) {
      if (value >= 1000000000000000ULL) { // number has 16 digits
        _mm_storeu_si128(reinterpret_cast<__m128i*>(result), digits_15_0);
        return 16;
      }
    }
    const __mmask16 mask = (__mmask16)(0xFFFFu << (16 - n));
    _mm_mask_storeu_epi8(result - 16 + n, mask, digits_15_0);
    return n;
  }

  if constexpr (V == Variant::Homogeneous) {
    const auto [q, r] = digits::div10e16(value); // 1..1844, 0..(10^16-1)
    char *p = digits::write_one_two_three_or_four_digits_10000(result, q);
    const __m128i digits_15_0 = to_string_avx512ifma(r);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(p), digits_15_0);
    return p - result + 16;
  } else { // Variant::Heterogeneous
    const auto [q, r] = digits::div10000<true>(value);  // full-range correct variant
    const uint32_t nq = n - 4;
    const __m128i v16 = to_string_avx512ifma(q);
    const __mmask16 mask = (__mmask16)(0xFFFFu << (16 - nq));
    _mm_mask_storeu_epi8(result - 16 + nq, mask, v16);
    digits::write_four_digits_10000(result + nq, r);
    return n;
  }
}
#endif // CHAMPAGNE_LEMIRE_AVX512

#endif // CHAMPAGNE_LEMIRE_H
