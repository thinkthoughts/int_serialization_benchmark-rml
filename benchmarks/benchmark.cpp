#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
using std::literals::string_literals::operator""s;

#include "performancecounters/benchmarker.h"
#include "champagne_lemire.h"
#include "dragonbox.h"

constexpr uint8_t FloatMantissaBits = 23;
constexpr uint8_t FloatExponentBits = 8;

constexpr uint8_t DoubleMantissaBits = 52;
constexpr uint8_t DoubleExponentBits = 11;

// mantissa * 10^exponent
struct decimal_float {
  uint64_t mantissa;
  int32_t exponent;
  bool sign;
};

decimal_float double_to_decimal_float(double value) {
  decimal_float result = {0, 0, false};

  // Handle zero
  if (value == 0.0) {
    return result;
  }

  // Extract sign
  result.sign = value < 0;
  value = std::abs(value);

  // Handle special cases: infinity and NaN
  if (std::isinf(value) || std::isnan(value)) {
    result.mantissa = 0;
    result.exponent = std::numeric_limits<uint32_t>::max();
    return result;
  }

  // Get exponent in base 10
  int exp10 = std::floor(std::log10(value));
  // Normalize value to [1, 10)
  double normalized = value / std::pow(10.0, exp10);
  // Adjust if normalized is 10 due to floating-point rounding
  if (normalized >= 10.0) {
    normalized /= 10.0;
    exp10++;
  }
  // Convert to mantissa with up to 17 digits (max for uint64_t)
  uint64_t mantissa =
      static_cast<uint64_t>(normalized * 100'000'000'000'000'000.0 + 0.5);
  exp10 -= 17; // Adjust exponent to account for the scaling factor
  // Remove trailing zeros
  while (mantissa % 10 == 0 && mantissa != 0) {
    mantissa /= 10;
    exp10++;
  }
  // We keep at most 17 digits in the mantissa
  while (mantissa > 100'000'000'000'000'000) {
    mantissa = (mantissa / 10) + (mantissa % 10 >= 5 ? 1 : 0); // naive rounding
    exp10++;
  }

  result.mantissa = mantissa;
  result.exponent = static_cast<int32_t>(exp10);
  return result;
}

void pretty_print(size_t volume, size_t bytes, const std::string &name,
                  event_aggregate agg) {
  fmt::print("{:<50} : ", name);
  fmt::print(" {:5.2f} ns/d ", agg.fastest_elapsed_ns() / volume);
  if (collector.has_events()) {
    fmt::print(" {:5.2f} GHz ",
               agg.fastest_cycles() / agg.fastest_elapsed_ns());
    fmt::print(" {:5.2f} c/d ", agg.fastest_cycles() / volume);
    fmt::print(" {:5.2f} i/d ", agg.fastest_instructions() / volume);
    fmt::print(" {:5.2f} i/B ", agg.fastest_instructions() / bytes);
    fmt::print(" {:5.2f} i/c ",
               agg.fastest_instructions() / agg.fastest_cycles());
  }
  fmt::print("\n");
}

template <typename T>
struct float_number_generator {
  virtual T new_float() = 0;
  virtual std::string describe() = 0;
  virtual ~float_number_generator() = default;
};

template <typename T>
struct uniform_generator : float_number_generator<T> {
  std::random_device rd;
  std::mt19937_64 gen;
  std::uniform_real_distribution<T> dis;
  explicit uniform_generator(T a = 0.0, T b = 1.0)
      : rd(), gen(rd()), dis(a, b) {}
  std::string describe() override {
    return "generate random numbers uniformly in the interval [" +
           std::to_string((dis.min)()) + std::string(",") +
           std::to_string((dis.max)()) + std::string("]");
  }
  T new_float() override { return dis(gen); }
};

std::vector<decimal_float> generate_large_set(size_t count = 1'000'000) {
  std::vector<decimal_float> result;
  uniform_generator<double> gen(-1e10, 1e10);
  result.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    result.push_back(double_to_decimal_float(gen.new_float()));
  }

  return result;
}

std::vector<double> read_floats_from_file(const std::string &filename) {
  std::vector<double> values;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    double val;
    if (iss >> val) {
      values.push_back(val);
    }
  }
  return values;
}

void compare_avx512_and_dragonbox(uint64_t mantissa, int32_t exponent) {
  char buffer[32];

  int n = avx512_to_chars(mantissa, exponent, buffer);
  buffer[n] = '\0';
  fmt::print("AVX-512:   {}\n", buffer);

  n = jkj::dragonbox::detail::to_chars(mantissa, exponent, buffer) - buffer;
  buffer[n] = '\0';
  fmt::print("Dragonbox: {}\n", buffer);
}

int main(int argc, char **argv) {
  std::vector<decimal_float> data;
  if (argc > 1) {
    // Lecture du fichier passé en argument
    auto floats = read_floats_from_file(argv[1]);
    if (floats.empty()) {
      fmt::print(stderr, "No valid floats found in the file.\n");
      return EXIT_FAILURE;
    }
    data.reserve(floats.size());
    for (double f : floats) {
      data.push_back(double_to_decimal_float(f));
    }
  } else {
    // Génération aléatoire par défaut
    data = generate_large_set();
  }
  fmt::print("Data size: {} floats\n", data.size());

  volatile uint64_t counter = 0;
  char buffer[128];

#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  auto avx512l = [&data, &counter, &buffer]() {
    for (size_t i = 0; i < data.size(); ++i) {
      char *start = buffer;
      if (data[i].sign) {
        buffer[0] = '-';
        start++;
      }
      counter = counter + avx512_to_chars(data[i].mantissa, data[i].exponent, start)
               + (data[i].sign ? 1 : 0);
    }
  };
  counter = 0;
  avx512l();
  size_t volume512 = counter;
  fmt::print("Volume 512: {}\n", volume512);
#endif
  auto drag = [&data, &counter, &buffer]() {
    using jkj::dragonbox::detail::to_chars;
    for (size_t i = 0; i < data.size(); ++i) {
      char *start = buffer;
      if (data[i].sign) {
        buffer[0] = '-';
        start++;
      }
      counter = counter + (to_chars(data[i].mantissa, data[i].exponent, start) - buffer)
               + (data[i].sign ? 1 : 0);
    }
  };
  counter = 0;
  drag();
  size_t volume_drag = counter;
  fmt::print("Volume drag: {}\n", volume_drag);

  for (size_t i = 0; i < 4; i++) {
    fmt::print("Run {}\n", i + 1);
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
    pretty_print(data.size(), volume512, "avx-512+champagne_lemire", bench(avx512l));
#endif
    pretty_print(data.size(), volume_drag, "dragonbox", bench(drag));
  }
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  fmt::print("Using AVX512IFMA\n");
#else
  fmt::print("Using fallback implementation (AVX-512 not found)\n");
#endif
}
