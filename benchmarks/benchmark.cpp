#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <fmt/core.h>
#include <cxxopts.hpp>
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

decimal_float double_to_decimal_float(double value, int mantissa_size = 17) {
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

  // If mantissa has more digits than mantissa_size, scale it down
  int current_digits = fast_digit_count(mantissa);
  while (current_digits > mantissa_size) {
    mantissa = (mantissa / 10) + (mantissa % 10 >= 5 ? 1 : 0); // naive rounding
    exp10++;
    current_digits--;
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

enum class DistributionMode {
  Uniform,  // Uniform distribution across digit lengths
  Natural   // Natural distribution (more high-digit numbers)
};

std::vector<decimal_float> generate_large_set(size_t count = 1'000'000,
                                              int min_digits = 1,
                                              int max_digits = 17,
                                              DistributionMode mode = DistributionMode::Uniform) {
  std::vector<decimal_float> result;
  result.reserve(count);

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_real_distribution<double> value_dist(-1e10, 1e10);
  std::uniform_int_distribution<int> uniform_digit_dist(min_digits, max_digits);

  // Create weights that exponentially favor higher digit counts
  std::vector<double> weights;
  double val = 1.0;
  for (int i = min_digits; i <= max_digits; ++i, val *= 16.0)
    weights.push_back(val);
  std::discrete_distribution<int> natural_digit_dist(weights.begin(), weights.end());

  for (size_t i = 0; i < count; ++i) {
    int mantissa_size = mode == DistributionMode::Natural
                      ? min_digits + natural_digit_dist(gen)
                      : uniform_digit_dist(gen);
    result.emplace_back(double_to_decimal_float(value_dist(gen), mantissa_size));
  }

  return result;
}

std::vector<double> read_floats_from_file(const std::string &filename) {
  std::vector<double> values;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    if (double val; iss >> val)
      values.push_back(val);
  }
  return values;
}

void compare_avx512_and_dragonbox(uint64_t mantissa, int32_t exponent) {
  char buffer[32];
  int n;

#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  n = avx512_to_chars(mantissa, exponent, buffer);
  buffer[n] = '\0';
  fmt::print("AVX-512:   {}\n", buffer);
#endif

  n = jkj::dragonbox::detail::to_chars(mantissa, exponent, buffer) - buffer;
  buffer[n] = '\0';
  fmt::print("Dragonbox: {}\n", buffer);
}

void test_some_harcoded_cases() {
  compare_avx512_and_dragonbox(12345678901234567ul, 20); // 17
  compare_avx512_and_dragonbox(123456789, 8); // 9
  compare_avx512_and_dragonbox(123456, 8); // 6
  compare_avx512_and_dragonbox(0, 1);
  compare_avx512_and_dragonbox(1, 1);
}

int main(int argc, char **argv) {
  cxxopts::Options options("benchmark", "Float to string conversion benchmark");

  options.add_options()
    ("h,help",  "Show help message")
    ("q,quick", "Do a quick validation test with some hardcoded cases")
    ("f,file",  "Input file containing floating point numbers", cxxopts::value<std::string>())
    ("n,num",   "Number of random numbers to generate", cxxopts::value<size_t>()->default_value("1000000"))
    ("m,min",   "Minimum mantissa digits for random generation (1-17)", cxxopts::value<int>()->default_value("1"))
    ("M,max",   "Maximum mantissa digits for random generation (1-17)", cxxopts::value<int>()->default_value("17"))
    ("d,distribution", "Distribution mode: 'uniform' (equal probability for each digit count)"
                       "or 'natural' (more high-digit numbers)", cxxopts::value<std::string>()->default_value("natural"));

  std::vector<decimal_float> data;
  try {
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      fmt::print("{}\n", options.help());
      fmt::print("\nExamples:\n");
      fmt::print("  {} -n 1000                 # Random 1000 numbers with 1-17 digit mantissas\n", argv[0]);
      fmt::print("  {} -f data/canada.txt      # Use data from file\n", argv[0]);
      fmt::print("  {} -m 1 -M 5               # Random data with 1-5 digit mantissas (uniform)\n", argv[0]);
      fmt::print("  {} --min=10 --max=17       # Random data with 10-17 digit mantissas (uniform)\n", argv[0]);
      fmt::print("  {} -d natural              # Natural distribution (more high-digit numbers)\n", argv[0]);
      fmt::print("  {} -m 5 -M 15 -d natural   # Natural distribution with 5-15 digit range\n", argv[0]);
      return EXIT_SUCCESS;
    }

    if (result.count("quick")) {
      test_some_harcoded_cases();
      return EXIT_SUCCESS;
    }

    size_t num_values = result["num"].as<size_t>();
    int min_digits = result["min"].as<int>();
    int max_digits = result["max"].as<int>();
    std::string distribution_str = result["distribution"].as<std::string>();

    // Parse distribution mode
    DistributionMode distribution_mode;
    if (distribution_str == "uniform") {
      distribution_mode = DistributionMode::Uniform;
    } else if (distribution_str == "natural") {
      distribution_mode = DistributionMode::Natural;
    } else {
      fmt::print(stderr, "Error: distribution must be 'uniform' or 'natural'\n");
      return EXIT_FAILURE;
    }

    // Validate digit ranges
    if (min_digits < 1 || min_digits > 17) {
      fmt::print(stderr, "Error: min_digits must be between 1 and 17\n");
      return EXIT_FAILURE;
    }
    if (max_digits < 1 || max_digits > 17) {
      fmt::print(stderr, "Error: max_digits must be between 1 and 17\n");
      return EXIT_FAILURE;
    }
    if (min_digits > max_digits) {
      fmt::print(stderr, "Error: min_digits ({}) cannot be greater than max_digits ({})\n", min_digits, max_digits);
      return EXIT_FAILURE;
    }

    if (result.count("file")) {
      // Load data from file
      const std::string filename = result["file"].as<std::string>();
      const auto floats = read_floats_from_file(filename);
      if (floats.empty()) {
        fmt::print(stderr, "No valid floats found in the file: {}\n", filename);
        return EXIT_FAILURE;
      }
      data.reserve(floats.size());
      for (double f : floats)
        data.push_back(double_to_decimal_float(f));
      fmt::print("Loaded {} floats from file: {}\n", data.size(), filename);
    } else {
      // Generate random data
      data = generate_large_set(num_values, min_digits, max_digits, distribution_mode);
      fmt::print("Generated {} random values with mantissa digits in range [{}, {}] using {} distribution\n",
                 num_values, min_digits, max_digits, distribution_str);
    }
  } catch (const cxxopts::exceptions::exception& e) {
    fmt::print(stderr, "Error parsing arguments: {}\n", e.what());
    fmt::print(stderr, "Use -h or --help for usage information.\n");
    return EXIT_FAILURE;
  }

  fmt::print("Data size: {} floats\n", data.size());

  // Print mantissa length distribution
  std::array<size_t, 17 + 1> mantissaDistrib{};
  for (const auto &df : data)
    ++mantissaDistrib[fast_digit_count(df.mantissa)];
  fmt::print("Mantissa length distribution:\n");
  for (size_t i = 1; i < mantissaDistrib.size(); ++i)
    fmt::print("\t{:2}: {}\n", i, mantissaDistrib[i]);

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
}
