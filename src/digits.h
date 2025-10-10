#ifndef FOURDIGITS_H
#define FOURDIGITS_H

#include <array>
#include <cstring>
#include <utility>
#include <cstdint>

#include "fullmultiplier.h"

namespace digits {

// This will divide by 100 using multiplication and shifts
// and leave a remainder that is not x%100 but that
// is still unique to x mod 100
// In this manner, we go from a number x in [0, 10000) to
// a pair (div, mod) using a single multiplication.
constexpr std::pair<uint64_t, uint64_t> div100v(uint64_t x) {
  const uint64_t v = x * uint64_t(0x028f5c29); // ceil(2^32 / 100)
  return {v >> 32, (v >> 24) & 0xff};
}

inline std::pair<uint64_t, uint64_t> div100(uint64_t x) {
  const uint64_t q = mul64x64_to_128(x, 0x28f5c28f5c28f5d).first; // ceil(2^64 / 100)
  return {q, x - 100 * q};
}

template <bool maybe_larger_than_10e15>
inline std::pair<uint64_t, uint64_t> div10000(uint64_t x) {
  const uint64_t q = mul64x64_to_128(x, 0x68db8bac710cc).first; // ceil(2^64 / 10000)
  const uint64_t r = x - 10000 * q;
  if constexpr (maybe_larger_than_10e15) {
    if (r > x)
      return {q - 1, r + 10000};
  }
  return {q, r};
}

inline std::array<char, 2> get_one_digit_with_dot(uint32_t value) {
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

inline std::array<char, 3> get_two_digits_with_dot(uint32_t value) {
  constexpr static std::array<std::array<char, 3>, 100> hundreds_digit_table =
    []() {
      std::array<std::array<char, 3>, 100> table{};
      for (int i = 0; i < 100; ++i) {
        table[i] = {static_cast<char>((i / 10) % 10 + '0'), '.',
                    static_cast<char>((i % 10) + '0')};
      }
      return table;
    }();
  return hundreds_digit_table[value];
}

inline std::array<char, 2> get_two_digits_v(uint32_t value) {
  constexpr static std::array<std::array<char, 2>, 256> hundreds_digit_table =
    []() {
      std::array<std::array<char, 2>, 256> table{};
      for (int i = 0; i < 10000; ++i) {
        table[div100v(i).second] = {static_cast<char>((i / 10) % 10 + '0'),
                                    static_cast<char>((i % 10) + '0')};
      }
      return table;
    }();
  return hundreds_digit_table[value];
}

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

inline void write_one_digit_with_dot(char *buffer, uint32_t value) {
  std::memcpy(buffer, get_one_digit_with_dot(value).data(), 2);
}

inline void write_two_digits_with_dot(char *buffer, uint32_t value) {
  std::memcpy(buffer, get_two_digits_with_dot(value).data(), 3);
}

inline void write_two_digits(char *buffer, uint32_t value) {
  std::memcpy(buffer, get_two_digits(value).data(), 2);
}

inline void write_three_digits(char *buffer, uint32_t value) {
  std::memcpy(buffer, get_three_digits(value).data(), 3);
}

inline void write_four_digits_10000(char *buffer, uint64_t value) {
  auto [high, low] = div100v(value);
  std::memcpy(buffer, get_two_digits(high).data(), 2);
  std::memcpy(buffer + 2, get_two_digits_v(low).data(), 2);
}

inline char* write_three_or_four_digits_10000(char *buffer, uint64_t value) {
  auto [high, low] = div100v(value);
  if (value < 1000) {
    buffer[0] = char('0' + high);
    std::memcpy(buffer + 1, get_two_digits_v(low).data(), 2);
    return buffer + 3;
  } else {
    std::memcpy(buffer,     get_two_digits(high).data(), 2);
    std::memcpy(buffer + 2, get_two_digits_v(low).data(), 2);
    return buffer + 4;
  }
}

inline char* write_one_two_three_or_four_digits_10000(char *buffer, uint64_t value) {
  if(value >= 1000) { // four digits
    const auto [high, low] = div100v(value);
    std::memcpy(buffer,     get_two_digits(high).data(), 2);
    std::memcpy(buffer + 2, get_two_digits_v(low).data(), 2);
    return buffer + 4;
  } else if(value >= 100) { // three digits
    // This could be further optimized:
    const auto [high, low] = div100v(value);
    buffer[0] = char('0' + high);
    std::memcpy(buffer + 1, get_two_digits_v(low).data(), 2);
    return buffer + 3;
  } else if(value >= 10) {
    std::memcpy(buffer, get_two_digits(value).data(), 2);
    return buffer + 2;
  } else {
    buffer[0] = char('0' + value);
    return buffer + 1;
  }
}

} // namespace digits

#endif
