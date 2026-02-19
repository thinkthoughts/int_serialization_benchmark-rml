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
//
// So div100v returns (x / 100, x % 100) but the second
// value is not exactly x % 100, it is a value unique to x mod 100
constexpr champagne_lemire_really_inline std::pair<uint64_t, uint64_t> div100v(uint64_t x) {
  const uint64_t v = x * uint64_t(0x28f5c29); // ceil(2^32 / 100)
  return {v >> 32, (v >> 24) & 0xff};
}

champagne_lemire_really_inline std::pair<uint64_t, uint64_t> div100(uint64_t x) {
  const uint64_t q = multiplier::mul64x64_to_128(x, 0x28f5c28f5c28f5d).first; // ceil(2^64 / 100)
  return {q, x - 100 * q};
}

enum {
  maybe_larger_than_1e15 = true,
  definitely_less_than_1e15 = false
};

template <bool maybe_larger_than_1e15>
champagne_lemire_really_inline std::pair<uint64_t, uint64_t> div10000(uint64_t x) {
  if constexpr (maybe_larger_than_1e15) {
    const uint64_t q = multiplier::mul64x64_to_128(x, 0x346dc5d63886594bull).first >> 11;
    const uint64_t r = x - 10000 * q;
    return {q, r};
  } else {
    const uint64_t q = multiplier::mul64x64_to_128(x, 0x68db8bac710cc).first; // ceil(2^64 / 10000)
    const uint64_t r = x - 10000 * q;
    return {q, r};
  }
}

// This is the constant g++15 generates (Granlund-Montgomery ?)
champagne_lemire_really_inline std::pair<uint64_t, uint64_t> div10e16(uint64_t x) {
  const uint64_t q = multiplier::mul64x64_to_128(x, 0x39A5652FB1137857ull).first >> 51;
  const uint64_t r = x - 10'000'000'000'000'000ull * q;
  return {q, r};
}

champagne_lemire_really_inline std::array<char, 2> get_two_digits_v(uint32_t value) {
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

champagne_lemire_really_inline std::array<char, 2> get_two_digits(uint32_t value) {
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

champagne_lemire_really_inline void write_four_digits_10000(char *buffer, uint64_t value) {
  auto [high, low] = div100v(value);
  std::memcpy(buffer, get_two_digits(high).data(), 2);
  std::memcpy(buffer + 2, get_two_digits_v(low).data(), 2);
}

champagne_lemire_really_inline char* write_one_two_three_or_four_digits_10000(char *buffer, uint64_t value) {
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
