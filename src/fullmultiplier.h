#ifndef FULLMULTIPLIER_H
#define FULLMULTIPLIER_H
#include <cstdint>
#include <utility>

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#elif defined(__SSE2__)
#include <x86intrin.h>
#endif // defined(_MSC_VER) && !defined(__clang__)

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
#elif __BMI2__
  long long unsigned int high;
  uint64_t low = _mulx_u64(a, b, &high);
  return {uint64_t(high), low};
#elif defined(__SIZEOF_INT128__)
  // GCC/Clang: Use __uint128_t
  __uint128_t result =
      static_cast<__uint128_t>(a) * static_cast<__uint128_t>(b);
  return {static_cast<uint64_t>(result >> 64), static_cast<uint64_t>(result)};
#else
  auto emulu = [](uint32_t x, uint32_t y) -> uint64_t {
    return x * (uint64_t)y;
  };

  // Split 64-bit numbers into 32-bit halves
  uint32_t a_lo = (uint32_t)a;
  uint32_t a_hi = (uint32_t)(a >> 32);
  uint32_t b_lo = (uint32_t)b;
  uint32_t b_hi = (uint32_t)(b >> 32);

  // Perform partial multiplications
  uint64_t ll = emulu(a_lo, b_lo);
  uint64_t lh = emulu(a_lo, b_hi);
  uint64_t hl = emulu(a_hi, b_lo);
  uint64_t hh = emulu(a_hi, b_hi);

  // Combine partial products with carry handling
  uint64_t cross = lh + hl;
  uint64_t cross_carry = (cross < lh);
  uint64_t low = ll + (cross << 32);
  uint64_t low_carry = (low < ll);
  uint64_t high = hh + (cross >> 32) + cross_carry + low_carry;
  return {high, low};
#endif
}

#endif // FULLMULTIPLIER_H
