#ifndef CHAMPAGNE_LEMIRE_FAST_H
#define CHAMPAGNE_LEMIRE_FAST_H

#include "digitcount.h"
#include "digits.h"
#include "portabilityutils.h"
#include <type_traits>

template <typename T>
champagne_lemire_really_inline int scalar_to_chars(T mantissa, int32_t exponent,
                                                 char *const result) {
  constexpr bool is_double = sizeof(T) == 8;
  static_assert(std::is_same_v<T, uint64_t> || std::is_same_v<T, uint32_t>,
                "Only integers are supported");
  static_assert(sizeof(T) == 8 || sizeof(T) == 4, "Unsupported type size");
  int32_t exp = exponent;
  size_t exp_index;
  if (mantissa == 0) {
    // Special case for zero.
    result[0] = '0';
    return 1;
  }

  if (mantissa >= 100'00'00'00'00'00'00'00) {
    // The mantissa is in [10^16, 10^17)
    size_t final_index = 17 + 1;
    uint64_t r1, r2, r3, r4;
    std::tie(mantissa, r1) =
        digits::div10000<digits::maybe_larger_than_1e15>(mantissa);
    digits::write_four_digits_10000(result + final_index - 4, r1);
    // The mantissa here should be in [10^12, 10^13)
    std::tie(mantissa, r2) =
        digits::div10000<digits::definitely_less_than_1e15>(mantissa);
    digits::write_four_digits_10000(result + final_index - 8, r2);
    // The mantissa here should be in [10^8, 10^9)
    std::tie(mantissa, r3) =
        digits::div10000<digits::definitely_less_than_1e15>(mantissa);
    digits::write_four_digits_10000(result + final_index - 12, r3);
    // The mantissa here should be in [10^4, 10^5)
    std::tie(mantissa, r4) =
        digits::div10000<digits::definitely_less_than_1e15>(mantissa);
    digits::write_four_digits_10000(result + final_index - 16, r4);
    digits::write_one_digit_with_dot(result, mantissa);
    exp += 16;
    exp_index = 17 + 1;
  } else {
    // 1 to 16
    const uint32_t number_of_digits =
        is_double ? fast_digit_count64(mantissa) : fast_digit_count32(mantissa);
    exp += number_of_digits - 1;
    size_t final_index = number_of_digits + 1;
    exp_index = final_index;
    if (mantissa >= 100'00'00'00) {
      // here we have at least 9 digits, up to 16 digits.
      // We are going to write the last 8 digits first.
      // So we shall have between 1 and 8 digits left to write.
      uint64_t r1, r2;
      std::tie(mantissa, r1) =
          digits::div10000<digits::maybe_larger_than_1e15>(mantissa);
      std::tie(mantissa, r2) =
          digits::div10000<digits::definitely_less_than_1e15>(mantissa);
      digits::write_four_digits_10000(result + final_index - 4, r1);
      digits::write_four_digits_10000(result + final_index - 8, r2);
      final_index -= 8;
    }
    // between 1 and 8 digits left to write.
    if (mantissa >= 100'00) {
      // We have 5 to 8 digits left to write.
      // We are going to write the last 4 digits first.
      // So we shall have between 1 and 4 digits left to write.
      uint64_t r1;
      std::tie(mantissa, r1) =
          digits::div10000<digits::definitely_less_than_1e15>(mantissa);
      digits::write_four_digits_10000(result + final_index - 4, r1);
      final_index -= 4;
    }
    if (mantissa >= 100) {
      // We have 3 to 4 digits left to write.
      // We are going to write the last 2 digits first.
      // So 1 to 2 digits left to write.
      uint64_t r;
      std::tie(mantissa, r) = digits::div100(mantissa);
      digits::write_two_digits(result + final_index - 2, r);
      final_index -= 2;
    }
    // We have one or two digits left to write.
    if (mantissa < 10) {
       digits::write_one_digit_with_dot(result, mantissa);
    } else {
      auto tens = (mantissa * 103) >> 10;
      digits::write_one_digit_with_dot(result, tens);
      result[2] = (mantissa - 10 * tens) + '0';
    }
    exp_index = number_of_digits + 1;
  }


  if (exp) { // We do not print the exponent if mantissa is zero but zero is
             // handled above.
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

#endif // CHAMPAGNE_LEMIRE_FAST_H