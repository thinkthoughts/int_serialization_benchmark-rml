#ifndef CHAMPAGNE_LEMIRE_H
#define CHAMPAGNE_LEMIRE_H

#include <bit>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#ifdef _MSC_VER
#ifdef __clang__
#define WE_HAVE_CLANGCL 1
#else
#define WE_HAVE_VISUAL_STUDIO 1
#include <intrin.h>
#endif
#endif

#ifdef __SSE2__
#include <x86intrin.h>
#endif

constexpr auto int_pow = [](uint64_t base, uint64_t exponent) -> uint64_t {
  uint64_t result = 1;
  while (exponent > 0) {
    if (exponent & 1)
      result *= base;
    base *= base;
    exponent >>= 1;
  }
  return result;
};

// Templated to_centi_digits function, temp should be in [0, 100**number_digits)
// (We do not check this, so it is the caller's responsibility)
template <int number_centi_digits>
inline __attribute__((always_inline)) std::array<int, number_centi_digits>
to_centi_digits(uint64_t temp) {
  static_assert(number_centi_digits > 0 && number_centi_digits <= 5,
                "number_digits must be between 1 and 5 inclusive");
  std::array<int, number_centi_digits> result;
  constexpr int number_of_most_significant_bits =
      number_centi_digits < 4 ? 32 : 8;
  constexpr int number_of_least_significant_bits =
      64 - number_of_most_significant_bits;
  constexpr uint64_t mask =
      UINT64_C(0xFFFFFFFFFFFFFFFF) >> number_of_most_significant_bits;
  constexpr uint64_t constant = ((1ULL << number_of_least_significant_bits) /
                                 int_pow(100, number_centi_digits - 1)) +
                                1;
  uint64_t full = temp * constant; // Scale to get the first digit
  result[0] = full >> number_of_least_significant_bits;

  for (int i : std::ranges::views::iota(1, number_centi_digits)) {
    full = (full & mask) * 100;
    result[i] = full >> number_of_least_significant_bits;
  }

  return result;
}

inline __attribute__((always_inline)) std::array<char, 2>
get_two_digits(uint32_t value) {
  constexpr static std::array<std::array<char, 2>, 100> hundreds_digit_table =
      []() {
        std::array<std::array<char, 2>, 100> table;
        for (int i = 0; i < 100; ++i) {
          // Calculate the tens digit
          table[i][0] = (i / 10) + '0';
          // Calculate the units digit
          table[i][1] = (i % 10) + '0';
        }
        return table;
      }();
  return hundreds_digit_table[value];
}

inline __attribute__((always_inline)) void write_two_digits(char *buffer,
                                                            uint32_t value) {
  std::memcpy(buffer, get_two_digits(value).data(), 2);
}

inline __attribute__((always_inline)) std::array<char, 2>
get_one_digits(uint32_t value) {
  constexpr static std::array<std::array<char, 2>, 10> digit_table = []() {
    std::array<std::array<char, 2>, 10> table;
    for (int i = 0; i < 10; ++i) {
      // Calculate the tens digit
      table[i][0] = i + '0';
      table[i][1] = '.';
    }
    return table;
  }();
  return digit_table[value];
}

inline __attribute__((always_inline)) void write_one_digits(char *buffer,
                                                            uint32_t value) {
  std::memcpy(buffer, get_one_digits(value).data(), 2);
}

////////////////////////
// We should use https://en.cppreference.com/w/cpp/numeric/countl_zero
////////////////////////
#if WE_HAVE_VISUAL_STUDIO
inline int leading_zeroes_64(uint64_t input_num) {
  unsigned long index;
#ifdef _WIN64 // highly recommended!!!
  _BitScanReverse64(&index, input_num);
#else  // if we must support 32-bit Windows
  if (input_num > 0xFFFFFFFF) {
    _BitScanReverse(&index, (uint32_t)(input_num >> 32));
    index += 32;
  } else {
    _BitScanReverse(&index, (uint32_t)(input_num));
  }
#endif // _WIN64
  return 63 - index;
}
#else
inline __attribute__((always_inline)) int
leading_zeroes_64(uint64_t input_num) {
  return __builtin_clzll(input_num);
}
#endif

inline __attribute__((always_inline)) int int_log2_64(uint64_t x) {
  return 63 - leading_zeroes_64(x | 1);
}

/**
 * Reference:
 * Daniel Lemire, "Computing the number of digits of an integer even faster," in
 * Daniel Lemire's blog, June 3, 2021,
 * https://lemire.me/blog/2021/06/03/computing-the-number-of-digits-of-an-integer-even-faster/.
 */
inline __attribute__((always_inline)) int fast_digit_count32(uint32_t x) {
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
 * Daniel Lemire, "Counting the digits of 64-bit integers," in Daniel Lemire's
 * blog, January 7, 2025,
 * https://lemire.me/blog/2025/01/07/counting-the-digits-of-64-bit-integers/.
 */
inline __attribute__((always_inline)) int fast_digit_count64(uint64_t x) {
  static uint64_t table[] = {9,
                             99,
                             999,
                             9999,
                             99999,
                             999999,
                             9999999,
                             99999999,
                             999999999,
                             9999999999,
                             99999999999,
                             999999999999,
                             9999999999999,
                             99999999999999,
                             999999999999999ULL,
                             9999999999999999ULL,
                             99999999999999999ULL,
                             999999999999999999ULL,
                             9999999999999999999ULL};
  int y = (19 * int_log2_64(x) >> 6);
  y += x > table[y];
  return y + 1;
}

// Adapted from the Ryu implementation.
template <typename T>
int to_chars(T mantissa, int32_t exponent, bool sign, char *const result) {
  constexpr bool is_double = sizeof(T) == 8;
  static_assert(is_double || sizeof(T) == 4, "Unsupported type size");
  int index = 0;
  if (sign)
    result[index++] = '-';
  // We use fast arithmetic to compute the number of digits.
  const uint32_t olength =
      is_double ? fast_digit_count64(mantissa) : fast_digit_count32(mantissa);
  uint32_t i = 0;
  if (mantissa >= 10'000'000'000'000'000) {
    const uint64_t q = mantissa / 100'000'000;
    uint64_t temp = mantissa % 100'000'000;
    mantissa = q;
    const uint64_t c = temp % 10000;
    temp /= 10000;
    const uint64_t d = temp % 10000;
    const uint64_t c0 = (c % 100);
    const uint64_t c1 = (c / 100);
    const uint64_t d0 = (d % 100);
    const uint64_t d1 = (d / 100);
    write_two_digits(result + index + olength - i - 1, c0);
    write_two_digits(result + index + olength - i - 3, c1);
    write_two_digits(result + index + olength - i - 5, d0);
    write_two_digits(result + index + olength - i - 7, d1); // 162.25 60 !!!
    i += 8;
  } else if (mantissa >= 100'000'000) {
    const uint64_t q = mantissa / 100'000'000;
    uint64_t temp = mantissa % 100'000'000;
    mantissa = q;
    const uint64_t c = temp % 10000;
    temp /= 10000;
    const uint64_t d = temp % 10000;
    const uint64_t c0 = (c % 100);
    const uint64_t c1 = (c / 100);
    const uint64_t d0 = (d % 100);
    const uint64_t d1 = (d / 100);
    write_two_digits(result + index + olength - i - 1, c0);
    write_two_digits(result + index + olength - i - 3, c1);
    write_two_digits(result + index + olength - i - 5, d0);
    write_two_digits(result + index + olength - i - 7, d1);
    i += 8;
  }
  uint64_t output = mantissa;
  // Next, we proceed in block of 4 digits.
  while (output >= 10'000) {
    const uint64_t c = output % 10000;
    output /= 10000;
    const uint64_t c0 = (c % 100);
    const uint64_t c1 = (c / 100);
    write_two_digits(result + index + olength - i - 1, c0);
    write_two_digits(result + index + olength - i - 3, c1);
    i += 4;
  }
  // We can take care of two digits out of the 2 or 3 remaining.
  if (output >= 100) {
    const uint64_t c = (output % 100);
    output /= 100;
    write_two_digits(result + index + olength - i - 1, c);
    i += 2;
  }
  // Last digit.
  if (output >= 10) {
    auto digits = get_two_digits(output);
    result[index + olength - i] = digits[1];
    result[index] = digits[0];
  } else {
    result[index] = (char)('0' + output);
  }

  // Print decimal point if needed.
  if (olength > 1) {
    result[index + 1] = '.';
    index += olength + 1;
  } else {
    ++index;
  }

  // Print the exponent.

  int32_t exp = exponent + (int32_t)olength - 1;
  if (mantissa && exp) { // We do not print the exponent if mantissa is zero.
    if (exp < 0) {
      result[index++] = 'E';
      result[index++] = '-';
      exp = -exp;
    } else {
      result[index++] = 'E';
    }

    if constexpr (is_double) {
      if (exp >= 100) { // 3 digits
        result[index++] = (char)('0' + exp / 100);
        exp %= 100;
        write_two_digits(result + index, exp);
      } else if (exp >= 10) { // 2 digits
        write_two_digits(result + index, exp);
      } else { // 1 digit
        result[index++] = (char)('0' + exp);
      }
    } else {
      if (exp >= 10) { // 2 digits
        write_two_digits(result + index, exp);
      } else { // 1 digit
        result[index++] = (char)('0' + exp);
      }
    }
  }

  return index;
}

std::pair<uint64_t, uint64_t> mul64x64_to_128(uint64_t a, uint64_t b) {
#if defined(_M_ARM64) && !defined(__MINGW32__)
  // ARM64 has native support for 64-bit multiplications, no need to emulate
  // But MinGW on ARM64 doesn't have native support for 64-bit multiplications
  return {__umulh(a, b), a * b};
#elif defined(_WIN64) && !defined(__clang__)
  // MSVC: Use _umul128 intrinsic
  uint64_t high;
  uint64_t low = _umul128(a, b, &high);
  return {high, low};
#elif defined(__SIZEOF_INT128__)
  // GCC/Clang: Use __uint128_t
#if __BMI2__
  long long unsigned int high;
  uint64_t low = _mulx_u64(a, b, &high);
  return {uint64_t(high), low};
#else
  __uint128_t result =
      static_cast<__uint128_t>(a) * static_cast<__uint128_t>(b);
  return {static_cast<uint64_t>(result >> 64), static_cast<uint64_t>(result)};
#endif
#else
  auto emulu = [](uint32_t x, uint32_t y) -> uint64_t {
    return x * (uint64_t)y;
  };
  uint64_t ad = emulu((uint32_t)(ab >> 32), (uint32_t)cd);
  uint64_t bd = emulu((uint32_t)ab, (uint32_t)cd);
  uint64_t adbc = ad + emulu((uint32_t)ab, (uint32_t)(cd >> 32));
  uint64_t adbc_carry = (uint64_t)(adbc < ad);
  uint64_t lo = bd + (adbc << 32);
  return {emulu((uint32_t)(ab >> 32), (uint32_t)(cd >> 32)) + (adbc >> 32) +
              (adbc_carry << 32) + (uint64_t)(lo < bd),
          lo};
#endif
}

#ifdef __aarch64__
std::pair<uint64_t, uint64_t> div100(uint64_t x) {
  auto [high, low] = mul64x64_to_128(x, 0x28f5c28f5c28f5d);
  return {high, mul64x64_to_128(low, 100).first};
} // 120 - 174
#else
std::pair<uint64_t, uint64_t> div100(uint64_t x) {
  auto [high, low] = mul64x64_to_128(x, 0x28f5c28f5c28f5d);
  return {high, x - 100 * high};
} // 128 - 190
#endif

#ifdef __aarch64__
// requires x <= 999999999999999 < 10**15
// return low bits
std::pair<uint64_t, uint64_t> div10000(uint64_t x) {
  auto [high, low] = mul64x64_to_128(x, 0x68db8bac710cc);
  return {high, low};
} // 120 - 174
#else
// requires x <= 999999999999999 < 10**15
// return low bits
std::pair<uint64_t, uint64_t> div10000(uint64_t x) {
  auto [high, low] = mul64x64_to_128(x, 0x68db8bac710cc);
  return {high, low};
} // 128 - 190
#endif

template <typename T>
int fast_to_chars(T mantissa, int32_t exponent, char *const result) {
  constexpr bool is_double = sizeof(T) == 8;
  static_assert(is_double || sizeof(T) == 4, "Unsupported type size");
  int32_t exp = exponent;
  size_t exp_index;
  if(mantissa == 0) {
    // Special case for zero.
    result[0] = '0';
    return 1;
  }

  if (mantissa >= 100'00'00'00'00'00'00'00) {
    size_t final_index = 17 + 1;
    uint64_t r1, r2, r3, r4;
    std::tie(mantissa, r1) = div10000(mantissa);
    std::tie(mantissa, r2) = div10000(mantissa);
    std::tie(mantissa, r3) = div10000(mantissa);
    std::tie(mantissa, r4) = div10000(mantissa);
    uint64_t high, low;
    std::tie(high, low) = mul64x64_to_128(r1, 100);
    write_two_digits(result + final_index - 2, high);
    std::tie(high, low) = mul64x64_to_128(low, 100);
    write_two_digits(result + final_index - 4, high);
    std::tie(high, low) = mul64x64_to_128(r2, 100);
    write_two_digits(result + final_index - 6, high);
    std::tie(high, low) = mul64x64_to_128(low, 100);
    write_two_digits(result + final_index - 8, high);
    std::tie(high, low) = mul64x64_to_128(r3, 100);
    write_two_digits(result + final_index - 10, high);
    std::tie(high, low) = mul64x64_to_128(low, 100);
    write_two_digits(result + final_index - 12, high);
    std::tie(high, low) = mul64x64_to_128(r4, 100);
    write_two_digits(result + final_index - 14, high);
    std::tie(high, low) = mul64x64_to_128(low, 100);
    write_two_digits(result + final_index - 16, high);
    write_one_digits(result, mantissa);
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
      std::tie(mantissa, r1) = div10000(mantissa);
      std::tie(mantissa, r2) = div10000(mantissa);
      uint64_t high, low;
      std::tie(high, low) = mul64x64_to_128(r1, 100);
      write_two_digits(result + final_index - 2, high);
      std::tie(high, low) = mul64x64_to_128(low, 100);
      write_two_digits(result + final_index - 4, high);
      std::tie(high, low) = mul64x64_to_128(r2, 100);
      write_two_digits(result + final_index - 6, high);
      std::tie(high, low) = mul64x64_to_128(low, 100);
      write_two_digits(result + final_index - 8, high);
      final_index -= 8;
    }
    // between 1 and 8 digits left to write.
    if (mantissa >= 100'00) {
      // We have 5 to 8 digits left to write.
      // We are going to write the last 4 digits first.
      // So we shall have between 1 and 4 digits left to write.
      uint64_t r1;
      std::tie(mantissa, r1) = div10000(mantissa);
      uint64_t high, low;
      std::tie(high, low) = mul64x64_to_128(r1, 100);
      write_two_digits(result + final_index - 2, high);
      std::tie(high, low) = mul64x64_to_128(low, 100);
      write_two_digits(result + final_index - 4, high);
      final_index -= 4;
    }
    if (mantissa >= 100) {
      // We have 3 to 4 digits left to write.
      // We are going to write the last 2 digits first.
      // So 1 to 2 digits left to write.
      uint64_t r;
      std::tie(mantissa, r) = div100(mantissa);
      write_two_digits(result + final_index - 2, r);
      final_index -= 2;
    }
    // We have one or two digits left to write.
    if (mantissa < 10) {
      if (number_of_digits == 1) {
        result[0] = (char)('0' + mantissa);
        exp_index = 1;
      } else {
        write_one_digits(result, mantissa);
      }
    } else {
      auto tens = (mantissa * 103) >> 10;
      write_one_digits(result, tens);
      result[2] = (mantissa - 10 * tens) + '0';
    }
  }

  if (exp) { // We do not print the exponent if mantissa is zero but zero is handled above.
    // About 20 instructions for the exponent?
    memcpy(result + exp_index, "E-", 2);
    exp_index += 1 + (exp < 0);

    if constexpr (is_double) {
      if (exp >= 100) { // 3 digits
        uint64_t prod = exp * 42949673;
        uint32_t head_digits = int(prod >> 32);
        result[exp_index++] = (char)('0' + head_digits);
        exp = exp - head_digits * 100;
        write_two_digits(result + exp_index, exp);
        exp_index += 3;
      } else { // 2 digits
        // If we need fewer than 2 digits, this will write a leading zero.
        write_two_digits(result + exp_index, exp);
        exp_index += 2;
      }
    } else {
      // If we need fewer than 2 digits, this will write a leading zero.
      write_two_digits(result + exp_index, exp);
      exp_index += 2;
    }
  }
  return exp_index;
}

template int to_chars<uint32_t>(uint32_t, int32_t, bool, char *const);
template int to_chars<uint64_t>(uint64_t, int32_t, bool, char *const);

#endif // CHAMPAGNE_LEMIRE_H