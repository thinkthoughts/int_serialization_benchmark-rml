#ifndef CHAMPAGNE_LEMIRE_H
#define CHAMPAGNE_LEMIRE_H
#include "fourdigits.h"
#include "fullmultiplier.h"
#include "digitcount.h"

#include <bit>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>



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

inline void write_one_digits(char *buffer,
                                                            uint32_t value) {
  std::memcpy(buffer, get_one_digits(value).data(), 2);
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
    // The mantissa is in [10^16, 10^17)
    size_t final_index = 17 + 1;
    uint64_t r1, r2, r3, r4;
    std::tie(mantissa, r1) = div10000(mantissa);
    // The mantissa here should be in [10^12, 10^13) 
    std::tie(mantissa, r2) = div10000(mantissa);
    // The mantissa here should be in [10^8, 10^9)
    std::tie(mantissa, r3) = div10000(mantissa);
    // The mantissa here should be in [10^4, 10^5)
    std::tie(mantissa, r4) = div10000(mantissa);
    // The mantissa here should be in [10^0, 10^1)
    //printf("mantissa: %llx, r1: %llx, r2: %llx, r3: %llx, r4: %llx\n", mantissa, r1, r2, r3, r4);
    uint64_t high, low;
    std::tie(high, low) = mul64x64_to_128(r1, 100);
    write_two_digits(result + final_index - 2, high);
    auto l = low;
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

template int fast_to_chars<uint32_t>(uint32_t, int32_t, char *const);
template int fast_to_chars<uint64_t>(uint64_t, int32_t, char *const);

#endif // CHAMPAGNE_LEMIRE_H