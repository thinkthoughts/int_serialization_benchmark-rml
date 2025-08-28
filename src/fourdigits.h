#ifndef FOURDIGITS_H
#define FOURDIGITS_H

#include <cstdio>
#include <array>
#include <cstring>
#include <utility>
#include <vector>
#include <cstdint>

namespace fourdigits {
// This will divide by 100 using multiplication and shifts
// and leave a remainder that is not x%100 but that
// is still unique to x mod 100
// In this manner, we go from a number x in [0, 10000) to
// a pair (div, mod) using a single multiplication.
constexpr std::pair<uint64_t, uint64_t> div100v(uint64_t x) {
    uint64_t v = x * uint64_t(0x28f5c29);
    return {v>>32, (v>>24)&0xff};
}



std::array<char, 2> get_two_digits(uint32_t value) {
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

std::array<char, 2> get_two_digits_v(uint32_t value) {
   constexpr static std::array<std::array<char, 2>, 256> hundreds_digit_table =
      []() {
        std::array<std::array<char, 2>, 256> table{};
        for (int i = 0; i < 10000; ++i) {
            table[div100v(i).second] = {static_cast<char>((i / 10)%10 + '0'), static_cast<char>((i % 10) + '0')};
        }
        return table;
      }();
  return hundreds_digit_table[value];
}
}

void write_four_digits_10000(char* buffer, uint64_t value) {
   auto [high, low] = fourdigits::div100v(value);
   std::memcpy(buffer, fourdigits::get_two_digits(high).data(), 2);
   std::memcpy(buffer + 2, fourdigits::get_two_digits_v(low).data(), 2);
}

#endif