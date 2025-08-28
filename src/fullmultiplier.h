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

#endif // FULLMULTIPLIER_H