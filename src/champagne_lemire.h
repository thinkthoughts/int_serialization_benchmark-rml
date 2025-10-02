#ifndef CHAMPAGNE_LEMIRE_H
#define CHAMPAGNE_LEMIRE_H

#include "digits.h"
#include "digitcount.h"
#include "ifma_avx512.h"

#include <cstdlib>
#include <sys/types.h>

#ifndef champagne_lemire_likely
#define champagne_lemire_likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef champagne_lemire_unlikely
#define champagne_lemire_unlikely(x) __builtin_expect(!!(x), 0)
#endif

#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512

// It is a SKETCH. It is likely not quite correct, but the spirit is there.
// Important: we inline the function.
template <typename T>
inline __attribute__((always_inline))
int avx512_to_chars(T mantissa, int32_t exponent, char *const result) {
  static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported type size");

  int32_t exp = exponent;
  size_t exp_index;
  if (mantissa >= 10'000'000'000'000'000) {
    // The special case where we max out the number of
    // digits exceeds 16 digits, and we handle it separately.
    // The mantissa is in [10^16, 10^17)
    // Ok, so we have to write 17 digits.
    uint64_t top_digit = mantissa / 10'000'000'000'000'000;
    digits::write_one_digit_with_dot(result, top_digit);

    // The call to to_string_avx512ifma and its storage amount to about
    // 25 instructions, and that can be about a third of the processing time.
    __m128i digits_15_0 = to_string_avx512ifma(mantissa % 10'000'000'000'000'000);
    _mm_storeu_si128((__m128i *)(result + 2), digits_15_0);
    exp += 16;
    exp_index = 18; // 17 digits + dot
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


int avx512_to_chars(uint64_t value, char *const result) {
  auto digits_15_0 = to_string_avx512ifma(value);
  const uint32_t number_of_digits = fast_digit_count(value);
  _mm_mask_storeu_epi8(result - 16 + number_of_digits, (1 << (number_of_digits)) - 1, digits_15_0);
  return number_of_digits;
}

#endif // CHAMPAGNE_LEMIRE_AVX512

#endif // CHAMPAGNE_LEMIRE_H
