#include "performancecounters/benchmarker.h"
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
using std::literals::string_literals::operator""s;
#include "champagne_lemire.h"
#include "dragonbox.h"

#include <vector>

constexpr uint8_t FloatMantissaBits = 23;
constexpr uint8_t FloatExponentBits = 8;

constexpr uint8_t DoubleMantissaBits = 52;
constexpr uint8_t DoubleExponentBits = 11;

struct IEEE754f {
  uint32_t mantissa;
  uint32_t exponent;
  bool sign;
};

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
  while (mantissa > 100000000000000000) {
    mantissa = (mantissa / 10) + (mantissa % 10 >= 5 ? 1 : 0); // naive rounding
    exp10++;
  }

  result.mantissa = mantissa;
  result.exponent = static_cast<int32_t>(exp10);
  return result;
}

void pretty_print(size_t volume, size_t bytes, const std::string& name,
                  event_aggregate agg) {
  fmt::print("{:<50} : ", name);
  fmt::print(" {:5.2f} ns/d ", agg.fastest_elapsed_ns() / volume);
  if (collector.has_events()) {
    fmt::print(" {:5.2f} GHz ", agg.fastest_cycles() / agg.fastest_elapsed_ns());
    fmt::print(" {:5.2f} c/d ", agg.fastest_cycles() / volume);
    fmt::print(" {:5.2f} i/d ", agg.fastest_instructions() / volume);
    fmt::print(" {:5.2f} i/c ", agg.fastest_instructions() / agg.fastest_cycles());
  }
  fmt::print("\n");
}

template <typename T> struct float_number_generator {
  virtual T new_float() = 0;
  virtual std::string describe() = 0;
  virtual ~float_number_generator() = default;
};
template <typename T> struct uniform_generator : float_number_generator<T> {
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

std::vector<decimal_float> generate_large_set(size_t count = 10'000) {
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
  size_t volume = data.size();
  volatile uint64_t counter = 0;
  char buffer[64];
  for (size_t i = 0; i < 4; i++) {
    fmt::print("Run {}\n", i + 1);
    pretty_print(data.size(), volume, "champagne_lemire",
                 bench([&data, &counter, &buffer]() {
                   for (size_t i = 0; i < data.size(); ++i) {
                     counter =
                         counter + to_chars(data[i].mantissa, data[i].exponent,
                                            data[i].sign, buffer);
                   }
                 }));
    pretty_print(data.size(), volume, "fast+champagne_lemire",
                 bench([&data, &counter, &buffer]() {
                   for (size_t i = 0; i < data.size(); ++i) {
                     char *start = buffer;
                     if (data[i].sign) {
                       buffer[0] = '-';
                       start++;
                     }
                     counter = counter +
                               fast_to_chars(data[i].mantissa, data[i].exponent,
                                             start) +
                               (data[i].sign ? 1 : 0);
                   }
                 }));
    pretty_print(data.size(), volume, "dragonbox",
                 bench([&data, &counter, &buffer]() {
                   for (size_t i = 0; i < data.size(); ++i) {
                     char *start = buffer;
                     if (data[i].sign) {
                       buffer[0] = '-';
                       start++;
                     }
                     counter = counter +
                               (jkj::dragonbox::detail::to_chars(
                                    data[i].mantissa, data[i].exponent, start) -
                                buffer);
                   }
                 }));
  }
}
