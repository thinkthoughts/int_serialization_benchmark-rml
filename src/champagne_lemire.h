#ifndef CHAMPAGNE_LEMIRE_H
#define CHAMPAGNE_LEMIRE_H
#include "fourdigits.h"
#include "fullmultiplier.h"
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

inline std::array<char, 2> get_two_digits(uint32_t value) {
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

inline void write_two_digits(char *buffer, uint32_t value) {
  std::memcpy(buffer, get_two_digits(value).data(), 2);
}

// We stop at 309 because that is the upper bound for the exponent in a double
inline std::array<char, 3> get_three_digits(uint32_t value) {
  constexpr static std::array<std::array<char, 3>, 309> digit_table =
    []() {
      std::array<std::array<char, 3>, 309> table;
      for (int i = 0; i < 309; ++i) {
        table[i][0] = (i / 100) + '0';
        // Calculate the tens digit
        table[i][1] = ((i / 10) % 10) + '0';
        // Calculate the units digit
        table[i][2] = (i % 10) + '0';
      }
      return table;
    }();
  return digit_table[value];
}

inline void write_three_digits(char *buffer, uint32_t value) {
  std::memcpy(buffer, get_three_digits(value).data(), 3);
}

inline std::array<char, 2>
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

inline void write_one_digits(char *buffer, uint32_t value) {
  std::memcpy(buffer, get_one_digits(value).data(), 2);
}

std::array<char, 3> get_two_digits_with_dot(uint32_t value) {
  constexpr static std::array<std::array<char, 3>, 100> hundreds_digit_table =
    []() {
      std::array<std::array<char, 3>, 100> table{};
      for (int i = 0; i < 100; ++i) {
        table[i] = {static_cast<char>((i / 10)%10 + '0'), '.', static_cast<char>((i % 10) + '0')};
      }
      return table;
    }();
  return hundreds_digit_table[value];
}

inline void write_two_digits_with_dot(char *buffer, uint32_t value) {
  std::memcpy(buffer, get_two_digits_with_dot(value).data(), 3);
}

#ifdef __aarch64__
std::pair<uint64_t, uint64_t> div100(uint64_t x) {
  auto [high, low] = mul64x64_to_128(x, 0x28f5c28f5c28f5d);
  return {high, mul64x64_to_128(low, 100).first};
} // 120 - 174
#else // ARM__aarch64__64
std::pair<uint64_t, uint64_t> div100(uint64_t x) {
  auto [high, low] = mul64x64_to_128(x, 0x28f5c28f5c28f5d);
  return {high, x - 100 * high};
} // 128 - 190
#endif //__aarch64__

#ifdef __aarch64__
// requires x <= 999999999999999 < 10**15
// return low bits
std::pair<uint64_t, uint64_t> div10000(uint64_t x) {
  auto [high, low] = mul64x64_to_128(x, 0x68db8bac710cc);
  return {high, low};
} // 120 - 174
#else // __aarch64__
// requires x <= 999999999999999 < 10**15
// return low bits
std::pair<uint64_t, uint64_t> div10000(uint64_t x) {
  auto [high, low] = mul64x64_to_128(x, 0x68db8bac710cc);
  return {high, low};
} // 128 - 190
#endif // __aarch64__


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
    size_t final_index = 17 + 1;
    // Ok, so we have to write 17 digits.
    uint64_t top_digit = mantissa / 10'000'000'000'000'000;
    write_one_digits(result, top_digit);
    // The call to to_string_avx512ifma and its storage amount to about
    // 25 instructions, and that can be about a third of the processing time.
    auto digits_15_0 = to_string_avx512ifma(mantissa % 10'000'000'000'000'000);
    _mm_storeu_si128((__m128i *)(result + 1), digits_15_0);
    exp += 16;
    exp_index = 18; // 17 digits + dot
  } else {
    // Next we do the general case.
    //
    // The mantissa is in [1,10^16)
    // When the mantissa is SHORT (few digits), the following is wasteful.
    // We could probably compute 8 digits faster. So we could branch here.
    auto digits_15_0 = to_string_avx512ifma(mantissa);
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
        write_three_digits(result + exp_index, exp);
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

#endif // CHAMPAGNE_LEMIRE_AVX512

#endif // CHAMPAGNE_LEMIRE_H
