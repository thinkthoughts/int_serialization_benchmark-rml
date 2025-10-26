#ifndef CHAMPAGNE_LEMIRE_FAST_H
#define CHAMPAGNE_LEMIRE_FAST_H

#include "digitcount.h"
#include "digits.h"
#include "portabilityutils.h"
#include <type_traits>
#include <tuple>
#include <utility>

template <typename T>
champagne_lemire_really_inline int scalar_to_chars(T mantissa, int32_t exponent,
                                                 char *const result) {
  //////////
  // LLVM/clang seems to like this function better than GCC.
  ////////
  //////////
  // For the math behind the multipliers, see:
  // scripts/optimal_bound.py
  ////////
  constexpr bool is_double = sizeof(T) == 8;
  static_assert(std::is_same_v<T, uint64_t> || std::is_same_v<T, uint32_t>,
                "Only integers are supported");
  static_assert(sizeof(T) == 8 || sizeof(T) == 4, "Unsupported type size");
  int32_t exp = exponent;
  const uint32_t number_of_digits = 
    is_double ? fast_digit_count64(mantissa) : fast_digit_count32(mantissa);
  size_t exp_index;
  if (mantissa >= 100'00'00'00'00'00'00'00) {
    // The mantissa is in [10^16, 10^17)
    // Important: GCC gets really confused with the registers if you don't write the
    // results to memory right away. Hence, I interleave the multiplications and the writes.
    //
    // We divide by 10^8, the binary remainder is in low10_8
    auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 48357032784585167ULL);
    low10_8 = (low10_8 >> 18) | (xdiv10_8 << (64 - 18));
    xdiv10_8 >>= 18;
    auto [digits12, low10_8_1] = multiplier::mul64x64_to_128(low10_8, 100);
    digits::write_two_digits(result + 10, digits12);
    auto [digits23, low10_8_2] = multiplier::mul64x64_to_128(low10_8_1, 100);
    digits::write_two_digits(result + 12, digits23);
    auto [digits45, low10_8_3] = multiplier::mul64x64_to_128(low10_8_2, 100);
    digits::write_two_digits(result + 14, digits45);
    auto [digits67, unused1] = multiplier::mul64x64_to_128(low10_8_3, 100);
    digits::write_two_digits(result + 16, digits67);
    // We divide by 10^8 again to get the first digits
    auto [xdiv10_16, low10_16] = multiplier::mul64x64_to_128(xdiv10_8, 184467440738ULL);
    digits::write_one_digit_with_dot(result, xdiv10_16);
    auto [digits89, low10_16_1] = multiplier::mul64x64_to_128(low10_16, 100);
    digits::write_two_digits(result + 2, digits89);
    auto [digits1011, low10_16_2] = multiplier::mul64x64_to_128(low10_16_1, 100);
    digits::write_two_digits(result + 4, digits1011);
    auto [digits1213, low10_16_3] = multiplier::mul64x64_to_128(low10_16_2, 100);
    digits::write_two_digits(result + 6, digits1213);
    auto [digits1415, unused2] = multiplier::mul64x64_to_128(low10_16_3, 100);
    digits::write_two_digits(result + 8, digits1415);
    exp += 16;
    exp_index = 17 + 1;
  } else {
    // 1 to 16 digits: determine number of digits and write accordingly.
    exp += number_of_digits - 1;
    size_t final_index = number_of_digits + 1;
    exp_index = final_index;

    ////////////////////
    // If <number_of_digits> is unpredictable, then we
    // get branch target buffer (BTB) misses.
    // This will lower the number of instructions per cycle (IPC).
    /////////////////
    ////////////////
    // The code below has poor naming conventions for the variables
    // and it is unclear. It should be rewritten for clarity.
    // The constants are from
    // scripts/optimal_bound.py
    ////////////////
    switch (number_of_digits) {
    case 16: {
      // We divide by 10^8, the binary remainder is in low10_8
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 48357032784585167ULL);
      low10_8 = (low10_8 >> 18) | (xdiv10_8 << (64 - 18));
      xdiv10_8 >>= 18;
      auto [digits12, low10_8_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 9, digits12);
      auto [digits23, low10_8_2] = multiplier::mul64x64_to_128(low10_8_1, 100);
      digits::write_two_digits(result + 11, digits23);
      auto [digits45, low10_8_3] = multiplier::mul64x64_to_128(low10_8_2, 100);
      digits::write_two_digits(result + 13, digits45);
      auto [digits67, unused1] = multiplier::mul64x64_to_128(low10_8_3, 100);
      digits::write_two_digits(result + 15, digits67);
      // We divide by 10^6 to get the first digits
      auto [xdiv10_14, low10_14] = multiplier::mul64x64_to_128(xdiv10_8, 18446744073710ULL);
      digits::write_two_digits_with_dot_with_one_pad(result, xdiv10_14);
      auto [digits89, low10_14_1] = multiplier::mul64x64_to_128(low10_14, 100);
      digits::write_two_digits(result + 3, digits89);
      auto [digits1011, low10_14_2] = multiplier::mul64x64_to_128(low10_14_1, 100);
      digits::write_two_digits(result + 5, digits1011);
      auto [digits1213, unused] = multiplier::mul64x64_to_128(low10_14_2, 100);
      digits::write_two_digits(result + 7, digits1213);
    } break;

    case 15: {
      // We divide by 10^8, the binary remainder is in low10_8
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 48357032784585167ULL);
      low10_8 = (low10_8 >> 18) | (xdiv10_8 << (64 - 18));
      xdiv10_8 >>= 18;
      auto [digits12, low10_8_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 8, digits12);
      auto [digits23, low10_8_2] = multiplier::mul64x64_to_128(low10_8_1, 100);
      digits::write_two_digits(result + 10, digits23);
      auto [digits45, low10_8_3] = multiplier::mul64x64_to_128(low10_8_2, 100);
      digits::write_two_digits(result + 12, digits45);
      auto [digits67, unused1] = multiplier::mul64x64_to_128(low10_8_3, 100);
      digits::write_two_digits(result + 14, digits67);
      // We divide by 10^6 to get the first digits
      auto [xdiv10_14, low10_14] = multiplier::mul64x64_to_128(xdiv10_8, 18446744073710ULL);
      digits::write_one_digit_with_dot(result, xdiv10_14);
      auto [digits89, low10_14_1] = multiplier::mul64x64_to_128(low10_14, 100);
      digits::write_two_digits(result + 2, digits89);
      auto [digits1011, low10_14_2] = multiplier::mul64x64_to_128(low10_14_1, 100);
      digits::write_two_digits(result + 4, digits1011);
      auto [digits1213, unused] = multiplier::mul64x64_to_128(low10_14_2, 100);
      digits::write_two_digits(result + 6, digits1213);
    } break;

    case 14: {
      // We divide by 10^8, the binary remainder is in low10_8
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 48357032784585167ULL);
      low10_8 = (low10_8 >> 18) | (xdiv10_8 << (64 - 18));
      xdiv10_8 >>= 18;
      auto [digits12, low10_8_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 7, digits12);
      auto [digits23, low10_8_2] = multiplier::mul64x64_to_128(low10_8_1, 100);
      digits::write_two_digits(result + 9, digits23);
      auto [digits45, low10_8_3] = multiplier::mul64x64_to_128(low10_8_2, 100);
      digits::write_two_digits(result + 11, digits45);
      auto [digits67, unused1] = multiplier::mul64x64_to_128(low10_8_3, 100);
      digits::write_two_digits(result + 13, digits67);
      // We divide by 10^4 to get the first digits
      auto [xdiv10_12, low10_12] = multiplier::mul64x64_to_128(xdiv10_8, 1844674407370956ULL);
      digits::write_two_digits_with_dot_with_one_pad(result, xdiv10_12);
      auto [digits89, low10_12_1] = multiplier::mul64x64_to_128(low10_12, 100);
      digits::write_two_digits(result + 3, digits89);
      auto [digits1011, unused] = multiplier::mul64x64_to_128(low10_12_1, 100);
      digits::write_two_digits(result + 5, digits1011);
    } break;

    case 13: {
      // We divide by 10^8, the binary remainder is in low10_8
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 48357032784585167ULL);
      low10_8 = (low10_8 >> 18) | (xdiv10_8 << (64 - 18));
      xdiv10_8 >>= 18;
      auto [digits12, low10_8_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 6, digits12);
      auto [digits23, low10_8_2] = multiplier::mul64x64_to_128(low10_8_1, 100);
      digits::write_two_digits(result + 8, digits23);
      auto [digits45, low10_8_3] = multiplier::mul64x64_to_128(low10_8_2, 100);
      digits::write_two_digits(result + 10, digits45);
      auto [digits67, unused1] = multiplier::mul64x64_to_128(low10_8_3, 100);
      digits::write_two_digits(result + 12, digits67);
      // We divide by 10^4 to get the first digits
      auto [xdiv10_12, low10_12] = multiplier::mul64x64_to_128(xdiv10_8, 1844674407370956ULL);
      digits::write_one_digit_with_dot(result, xdiv10_12);
      auto [digits89, low10_12_1] = multiplier::mul64x64_to_128(low10_12, 100);
      digits::write_two_digits(result + 2, digits89);
      auto [digits1011, unused] = multiplier::mul64x64_to_128(low10_12_1, 100);
      digits::write_two_digits(result + 4, digits1011);
    } break;

    case 12: {
      // We divide by 10^6, the binary remainder is in low10_6
      auto [xdiv10_6, low10_6] = multiplier::mul64x64_to_128(mantissa, 18446744073710ULL);
      auto [digits12, low10_6_1] = multiplier::mul64x64_to_128(low10_6, 100);
      digits::write_two_digits(result + 7, digits12);
      auto [digits23, low10_6_2] = multiplier::mul64x64_to_128(low10_6_1, 100);
      digits::write_two_digits(result + 9, digits23);
      auto [digits45, low10_6_3] = multiplier::mul64x64_to_128(low10_6_2, 100);
      digits::write_two_digits(result + 11, digits45);
      // We divide by 10^4 to get the first digits
      auto [xdiv10_10, low10_12] = multiplier::mul64x64_to_128(xdiv10_6, 1844674407370956ULL);
      digits::write_two_digits_with_dot_with_one_pad(result, xdiv10_10);
      auto [digits89, unused] = multiplier::mul64x64_to_128(low10_12, 100);
      digits::write_two_digits(result + 3, digits89);
      auto [digits67, unused1] = multiplier::mul64x64_to_128(low10_12, 100);
      digits::write_two_digits(result + 5, digits67);
    } break;

    case 11: {
      // We divide by 10^6, the binary remainder is in low10_6
      auto [xdiv10_6, low10_6] = multiplier::mul64x64_to_128(mantissa, 18446744073710ULL);
      auto [digits12, low10_6_1] = multiplier::mul64x64_to_128(low10_6, 100);
      digits::write_two_digits(result + 6, digits12);
      auto [digits23, low10_6_2] = multiplier::mul64x64_to_128(low10_6_1, 100);
      digits::write_two_digits(result + 8, digits23);
      auto [digits45, low10_6_3] = multiplier::mul64x64_to_128(low10_6_2, 100);
      digits::write_two_digits(result + 10, digits45);
      // We divide by 10^4 to get the first digits
      auto [xdiv10_10, low10_12] = multiplier::mul64x64_to_128(xdiv10_6, 1844674407370956);
      digits::write_one_digit_with_dot(result, xdiv10_10);
      auto [digits89, unused] = multiplier::mul64x64_to_128(low10_12, 100);
      digits::write_two_digits(result + 2, digits89);
      auto [digits67, unused1] = multiplier::mul64x64_to_128(low10_12, 100);
      digits::write_two_digits(result + 4, digits67);
    } break;
    case 10: {
      // We divide by 10^8, the binary remainder is in low10_8
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 184467440738ULL);
      digits::write_two_digits_with_dot_with_one_pad(result, xdiv10_8);
      auto [digits12, low10_6_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 3, digits12);
      auto [digits23, low10_6_2] = multiplier::mul64x64_to_128(low10_6_1, 100);
      digits::write_two_digits(result + 5, digits23);
      auto [digits45, low10_6_3] = multiplier::mul64x64_to_128(low10_6_2, 100);
      digits::write_two_digits(result + 7, digits45);
      auto [digits67, unused1] = multiplier::mul64x64_to_128(low10_6_3, 100);
      digits::write_two_digits(result + 9, digits67);
    } break;

    case 9: {
      // We divide by 10^8, the binary remainder is in low10_8
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 184467440738ULL);
      digits::write_one_digit_with_dot(result, xdiv10_8);
      auto [digits12, low10_6_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 2, digits12);
      auto [digits23, low10_6_2] = multiplier::mul64x64_to_128(low10_6_1, 100);
      digits::write_two_digits(result + 4, digits23);
      auto [digits45, low10_6_3] = multiplier::mul64x64_to_128(low10_6_2, 100);
      digits::write_two_digits(result + 6, digits45);
      auto [digits67, unused1] = multiplier::mul64x64_to_128(low10_6_3, 100);
      digits::write_two_digits(result + 8, digits67);
    } break;

    case 8: {
      // We divide by 10^6, the binary remainder is in low10_6
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 18446744073710ULL);
      digits::write_two_digits_with_dot_with_one_pad(result, xdiv10_8);
      auto [digits12, low10_6_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 3, digits12);
      auto [digits23, low10_6_2] = multiplier::mul64x64_to_128(low10_6_1, 100);
      digits::write_two_digits(result + 5, digits23);
      auto [digits45, unused] = multiplier::mul64x64_to_128(low10_6_2, 100);
      digits::write_two_digits(result + 7, digits45);
    } break;

    case 7: {
      // We divide by 10^6, the binary remainder is in low10_6
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 18446744073710ULL);
      digits::write_one_digit_with_dot(result, xdiv10_8);
      auto [digits12, low10_6_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 2, digits12);
      auto [digits23, low10_6_2] = multiplier::mul64x64_to_128(low10_6_1, 100);
      digits::write_two_digits(result + 4, digits23);
      auto [digits45, unused] = multiplier::mul64x64_to_128(low10_6_2, 100);
      digits::write_two_digits(result + 6, digits45);

    } break;

    case 6: {
      // We divide by 10^4, the binary remainder is in low10_8
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 1844674407370956ULL);
      digits::write_two_digits_with_dot_with_one_pad(result, xdiv10_8);
      auto [digits12, low10_6_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 3, digits12);
      auto [digits23, unused] = multiplier::mul64x64_to_128(low10_6_1, 100);
      digits::write_two_digits(result + 5, digits23);
    } break;

    case 5: {
      // We divide by 10^4, the binary remainder is in low10_4
      auto [xdiv10_8, low10_8] = multiplier::mul64x64_to_128(mantissa, 1844674407370956ULL);
      digits::write_one_digit_with_dot(result, xdiv10_8);
      auto [digits12, low10_6_1] = multiplier::mul64x64_to_128(low10_8, 100);
      digits::write_two_digits(result + 2, digits12);
      auto [digits23, unused] = multiplier::mul64x64_to_128(low10_6_1, 100);
      digits::write_two_digits(result + 4, digits23);
    } break;

    case 4: {
      // remaining 4 digits: write 2 + 2 with dot
      auto [r, s_unique] = digits::div100v(mantissa);
      digits::write_two_digits_with_dot_with_one_pad(result, r);
      digits::write_two_digits_v(result + final_index - 2, s_unique);
    } break;

    case 3: {
      // remaining 3 digits: write 2 + 1 with dot
      auto [r, s_unique] = digits::div100v(mantissa);
      digits::write_two_digits_v(result + final_index - 2, s_unique);
      digits::write_one_digit_with_dot(result, r);
    } break;

    case 2: {
      // remaining 2 digits: write 2 with dot
      digits::write_two_digits_with_dot(result, mantissa);
    } break;

    case 1: {
      *result = '0' + mantissa;
      if(mantissa == 0) {
        return 1;
      }
    } break;

    default:
      std::unreachable();
    }
  }


  if (exp) { // We do not print the exponent if mantissa is zero but zero is
             // handled above.
    // About 20 instructions for the exponent?
    memcpy(result + exp_index, "E-", 2);
    if(exp < 0) {
      exp_index += 2;
      exp = -exp;
    } else {
      exp_index += 1;
    }

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