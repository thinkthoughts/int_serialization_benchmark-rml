#ifndef INTEGER_CONV_BASELINES_H
#define INTEGER_CONV_BASELINES_H

#include <array>
#include "digitcount.h"

namespace baselines_int {

champagne_lemire_really_inline int naive(uint64_t value, char *const result) {
  const int number_of_digits = fast_digit_count(value);
  uint32_t i = number_of_digits;
  while(i--) {
    const uint64_t digit = value % 10;
    value /= 10;
    result[i] = '0' + digit;
  }
  return number_of_digits;
}

} // namespace baselines_int

#endif
