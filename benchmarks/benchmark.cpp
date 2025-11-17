#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <random>
#include <numeric>
#include <sstream>
#include <string>
#include <variant>
#include  <vector>
#include <print>
#include <cxxopts.hpp>
using std::literals::string_literals::operator""s;

#include "performancecounters/benchmarker.h"
#include "champagne_lemire.h"
#include "dragonbox.h"
#include "scalar.h"
#include "integer_conv_baselines.h"

constexpr size_t Number_Benchmark_Runs = 1;
constexpr double Ratio_To_Sample = 0.01;
constexpr double Ratio_Homogeneous = 0.95; // Homogeneous mode if > 95% of
                                           // numbers have the same digit length

// mantissa * 10^exponent
struct decimal_float {
  uint64_t mantissa;
  int32_t exponent;
  bool sign;
};

// return the exponent and mantissa of the decimal representation of num_str
// should be a non-negative number.
std::pair<uint64_t,int64_t> find_mantissa_exponent(std::string_view num_str) {
  uint64_t mantissa = 0;
  size_t after_dot = 0;
  bool found_dot = false;


  size_t idx = 0;

  for (; idx < num_str.size(); ++idx) {
    const char c = num_str[idx];
    if (c == '.' && !found_dot) {
      found_dot = true;
      continue;
    }
    if (std::isdigit(static_cast<unsigned char>(c))) {
      mantissa = mantissa * 10 + (c - '0');
      if(found_dot) {
        after_dot++;
      }
    } else {
      break;
    }
  }
  int64_t exponent_part = -static_cast<int64_t>(after_dot);
  if (idx < num_str.size() && (num_str[idx] == 'e' || num_str[idx] == 'E')) {
    ++idx;
    bool negative_exponent = false;
    if (idx < num_str.size() && (num_str[idx] == '+' || num_str[idx] == '-')) {
      negative_exponent = (num_str[idx] == '-');
      ++idx;
    }
    for (; idx < num_str.size(); ++idx) {
      const char c = num_str[idx];
      if (std::isdigit(static_cast<unsigned char>(c))) {
        exponent_part = exponent_part * 10 + (c - '0');
      } else {
        break;
      }
    }
    if (negative_exponent) {
      exponent_part = -exponent_part;
    }
  }
  if(mantissa == 0 ) {
    exponent_part = 0;
  }
  while(mantissa % 10 == 0 && mantissa != 0) {
    mantissa /= 10;
    exponent_part++;
  }
  return {mantissa, exponent_part};
}

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

  char buf[32]{};
  auto from_char_result = std::to_chars(buf, buf + sizeof(buf), value);
  if (from_char_result.ec != std::errc()) {
    // Handle error
    throw std::runtime_error("Error converting double to chars");
  }
  auto [mantissa, exponent_part] = find_mantissa_exponent(std::string_view(buf, from_char_result.ptr - buf));

  // If mantissa has more digits than mantissa_size, scale it down
  int current_digits = fast_digit_count(mantissa);
  if(current_digits > 17) {
    std::println("Warning: mantissa has more than 17 digits: {}", mantissa);
  }
  while (current_digits > mantissa_size) {
    mantissa = (mantissa / 10) + (mantissa % 10 >= 5 ? 1 : 0); // naive rounding
    exponent_part++;
    current_digits--;
  }
  result.mantissa = mantissa;
  result.exponent = static_cast<int32_t>(exponent_part);
  return result;
}

void pretty_print(size_t volume, size_t bytes, const std::string &name,
                  event_aggregate agg) {
  std::print("{:<50} : ", name);
  std::print(" {:5.2f} ns/d ", agg.fastest_elapsed_ns() / volume);
  if (collector.has_events()) {
    std::print(" {:5.2f} GHz ",
               agg.fastest_cycles() / agg.fastest_elapsed_ns());
    std::print(" {:5.2f} c/d ", agg.fastest_cycles() / volume);
    std::print(" {:5.2f} i/d ", agg.fastest_instructions() / volume);
    std::print(" {:5.2f} B/d ", agg.branches() / volume);
    std::print(" {:5.2f} BM/d ", agg.branch_misses() / volume);
    std::print(" {:5.2f} i/B ", agg.fastest_instructions() / bytes);
    std::print(" {:5.2f} i/c ",
               agg.fastest_instructions() / agg.fastest_cycles());
  }
  std::print("\n");
}

enum class DistributionMode {
  Uniform,  // Uniform distribution across digit lengths
  Natural   // Natural distribution (more high-digit numbers)
};

template<typename T>
std::vector<T> generate_large_set(size_t count = 1'000'000,
                                  int min_digits = 1, int max_digits = 17,
                                  DistributionMode mode = DistributionMode::Uniform) {
  std::vector<T> result;
  result.reserve(count);

  std::random_device rd;
  std::mt19937_64 gen(rd());

  // Create weights that exponentially favor higher digit counts
  std::vector<double> weights;
  double val = 1.0;
  for (int i = min_digits; i <= max_digits; ++i, val *= 32.0)
    weights.push_back(val);

  std::uniform_int_distribution<int> uniform_digit_dist(min_digits, max_digits);
  std::discrete_distribution<int> natural_digit_dist(weights.begin(), weights.end());
  auto pick_digits = [&](DistributionMode m) {
    return m == DistributionMode::Natural
             ? min_digits + natural_digit_dist(gen)
             : uniform_digit_dist(gen);
  };

  if constexpr (std::is_same_v<T, decimal_float>) {
    std::uniform_real_distribution<double> value_dist(-1e10, 1e10);
    for (size_t i = 0; i < count; ++i) {
      int mantissa_size = pick_digits(mode);
      decimal_float df;
      do {
        df = double_to_decimal_float(value_dist(gen), mantissa_size);
      } while (fast_digit_count(df.mantissa) != mantissa_size);
      result.emplace_back(df);
    }
  } else if constexpr (std::is_same_v<T, uint64_t>) {
    std::array<uint64_t, 20 + 1> pow10;
    pow10[0] = 1;
    for (int i = 1; i <= 20; ++i)
      pow10[i] = pow10[i - 1] * 10;

    for (size_t i = 0; i < count; ++i) {
      int digits = pick_digits(mode);
      uint64_t lower = pow10[digits - 1];
      uint64_t upper = digits < 20
                     ? pow10[digits] - 1
                     : std::numeric_limits<uint64_t>::max();
      std::uniform_int_distribution<uint64_t> dis(lower, upper);
      result.push_back(dis(gen));
    }
  }

  return result;
}

template <typename T>
std::vector<T> read_from_file(const std::string &filename) {
  std::vector<T> values;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    if (T val; iss >> val)
      values.push_back(val);
  }
  return values;
}

bool compare_decimal_floats_algorithms(uint64_t mantissa, int32_t exponent) {
  std::print("\nComparing mantissa={} exponent={}\n", mantissa, exponent);
  char buffer[32];
  std::fill(buffer, buffer + sizeof(buffer), 0);
  int n;
  n = scalar_to_chars(mantissa, exponent, buffer);
  buffer[n] = '\0';
  std::string fastans = buffer;

  std::print("fast:      {}\n", fastans);
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  n = avx512_to_chars(mantissa, exponent, buffer);
  buffer[n] = '\0';
  std::string avx512ans = buffer;
  // Short normalization: remove a single '0' after E/e and optional sign (E09 -> E9)
  std::print("AVX-512:   {}\n", avx512ans);

#endif

  n = jkj::dragonbox::detail::to_chars(mantissa, exponent, buffer) - buffer;
  buffer[n] = '\0';
  std::print("Dragonbox: {}\n", buffer);
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  std::string dragonans = buffer;
  if (avx512ans != dragonans) {
    std::print("Mismatch with Dragonbox: {}\n", dragonans);
    // allow an extra 0 after E if the exponent is one digit
    if (auto pos = avx512ans.find_first_of("Ee"); pos != std::string::npos) {
      size_t j = pos + 1;
      if (j < avx512ans.size() && (avx512ans[j] == '+' || avx512ans[j] == '-')) ++j;
      if (j + 1 < avx512ans.size() && avx512ans[j] == '0' && isdigit(static_cast<unsigned char>(avx512ans[j+1]))) {
        avx512ans.erase(j, 1);
        std::print("AVX-512:   {}\n", avx512ans);
      }
    }
    return avx512ans == dragonans;
  }
#endif
  return true;
}

bool compare_integers_algorithms(uint64_t number) {
  std::print("\nComparing number={}\n", number);
  char buffer[32];
  std::fill(buffer, buffer + sizeof(buffer), 0);
  int n;

#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  n = avx512_to_chars<Variant::Homogeneous>(number, buffer);
  buffer[n] = '\0';
  std::string avx512homoans = buffer;
  std::print("AVX-512 Homogeneous:   {}\n", buffer);
  n = avx512_to_chars<Variant::Heterogeneous>(number, buffer);
  buffer[n] = '\0';
  std::string avx512heteroans = buffer;
  std::print("AVX-512 Heterogeneous: {}\n", buffer);
#endif
  n = baselines_int::naive(number, buffer);
  buffer[n] = '\0';
  std::print("naive_onepass:         {}\n", buffer);

  n = baselines_int::mathisen_sse(number, buffer);
  buffer[n] = '\0';
  std::print("mathisen_sse:          {}\n", buffer);

  n = baselines_int::mula_sse64(number, buffer);
  buffer[n] = '\0';
  std::print("mula_sse64:            {}\n", buffer);

  n = baselines_int::hopman_fast(number, buffer);
  buffer[n] = '\0';
  std::print("hopman_fast:           {}\n", buffer);

  n = std::to_chars(buffer, buffer + sizeof(buffer), number).ptr - buffer;
  buffer[n] = '\0';
  std::print("std::to_chars:         {}\n", buffer);
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  std::string stdans = buffer;
  if (avx512homoans != stdans) {
    std::print("Mismatch between AVX-512<Homo> and std::to_chars: {} != {}\n", stdans, avx512homoans);
    std::print("==========================================================\n");
    return false;
  }
  if (avx512heteroans != stdans) {
    std::print("Mismatch between AVX-512<Hetero> and std::to_chars: {} != {}\n", stdans, avx512heteroans);
    std::print("============================================================\n");
    return false;
  }
#endif
  return true;
}

bool test_some_harcoded_floats() {
  bool result = true;

  result &= compare_decimal_floats_algorithms(789789974432, 20);
  result &= compare_decimal_floats_algorithms(12345678901234567ul, 20); // 17
  result &= compare_decimal_floats_algorithms(1234567890123456ul, 19); // 16
  result &= compare_decimal_floats_algorithms(123456789012345ul, 18); // 15
  result &= compare_decimal_floats_algorithms(12345678901234ul, 17); // 14
  result &= compare_decimal_floats_algorithms(1234567890123ul, 16); // 13
  result &= compare_decimal_floats_algorithms(123456789012ul, 15); // 12
  result &= compare_decimal_floats_algorithms(12345678901ul, 14); // 11
  result &= compare_decimal_floats_algorithms(1234567890ul, 13); // 10
  result &= compare_decimal_floats_algorithms(123456789, 9); // 9
  result &= compare_decimal_floats_algorithms(12345678, 8);
  result &= compare_decimal_floats_algorithms(1234567, 7);
  result &= compare_decimal_floats_algorithms(123456, 6);
  result &= compare_decimal_floats_algorithms(12345, 5);
  result &= compare_decimal_floats_algorithms(1236, 4);
  result &= compare_decimal_floats_algorithms(123, 3);
  result &= compare_decimal_floats_algorithms(12, 2);
  result &= compare_decimal_floats_algorithms(1, 1);
  result &= compare_decimal_floats_algorithms(0, 1);
  result &= compare_decimal_floats_algorithms(1, 1);
  return result;
}

bool test_some_harcoded_integers() {
  std::printf("Running hardcoded integer tests\n");
  bool result = true;
  result &= compare_integers_algorithms(12345678901234567890ull); // 20
  result &= compare_integers_algorithms(1234567890123456789ull); // 19
  result &= compare_integers_algorithms(123456789012345678ull); // 18
  result &= compare_integers_algorithms(12345678901234567ull); // 17
  result &= compare_integers_algorithms(1234567890123456ull); // 16
  result &= compare_integers_algorithms(123456789012345ull); // 15
  result &= compare_integers_algorithms(12345678901234ull); // 14
  result &= compare_integers_algorithms(1234567890123ull); // 13
  result &= compare_integers_algorithms(123456789012ull); // 12
  result &= compare_integers_algorithms(12345678901ull); // 11
  result &= compare_integers_algorithms(1234567890ull); // 10
  result &= compare_integers_algorithms(123456789ull); // 9
  result &= compare_integers_algorithms(12345678ull); // 8
  result &= compare_integers_algorithms(1234567ull); // 7
  result &= compare_integers_algorithms(123456ull); // 6
  result &= compare_integers_algorithms(12345ull); // 5
  result &= compare_integers_algorithms(1234ull); // 4
  result &= compare_integers_algorithms(123ull); // 3
  result &= compare_integers_algorithms(12ull); // 2
  result &= compare_integers_algorithms(1ull); // 1
  result &= compare_integers_algorithms(0);
  return result;
}

template<typename T>
Variant detect_variant(const std::vector<T> &data) {
  const auto start = std::chrono::steady_clock::now();

  const size_t sample_size = Ratio_To_Sample * size(data);
  std::array<size_t, 21> lengthDistrib{};
  for (size_t i = 0; i < sample_size; ++i) {
    const T &v = data[i];
    uint64_t number = [&] {
      if constexpr (std::is_same_v<std::decay_t<decltype(v)>, decimal_float>)
        return v.mantissa;
      else
        return v;
    }();
    ++lengthDistrib[fast_digit_count(number)];
  }

  const size_t total = std::reduce(lengthDistrib.begin(),
                                   lengthDistrib.end(), size_t{0});
  const size_t dominant_length = *std::max_element(lengthDistrib.begin(), lengthDistrib.end());
  const double dominant_ratio = static_cast<double>(dominant_length) / static_cast<double>(total);

  const auto end = std::chrono::steady_clock::now();
  const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::print("Variant detection took {} µs\n", elapsed);

  return dominant_ratio >= Ratio_Homogeneous
         ? Variant::Homogeneous  // currently optimized for 1-4, 8, 16-20
         : Variant::Heterogeneous;
}

template<typename T>
void run_benchmark(const std::vector<T> &data, [[maybe_unused]] Variant algo_variant = Variant::Auto) {
  uint64_t counter = 0;
  char buffer[128];

  // --- Helper to pretty-print run results ---
  auto run_and_report = [&](auto&& name, auto&& func, size_t volume) {
    std::print("\n");
    for (size_t i = 0; i < Number_Benchmark_Runs; ++i)
      pretty_print(data.size(), volume, name, bench(func));
  };

  if constexpr (std::is_same_v<T, decimal_float>) {
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
    auto avx512l = [&data, &counter, &buffer]() {
      for (size_t i = 0; i < data.size(); ++i) {
        char *start = buffer;
        buffer[0] = '-';
        start += data[i].sign ? 1 : 0;
        counter += avx512_to_chars(data[i].mantissa, data[i].exponent, start)
                 + (data[i].sign ? 1 : 0);
      }
    };
    counter = 0;
    avx512l();
    size_t volume512 = counter;
    std::print("Volume 512: {}\n", volume512);
#endif
    auto scalar = [&data, &counter, &buffer]() {
      for (size_t i = 0; i < data.size(); ++i) {
        char *start = buffer;
        buffer[0] = '-';
        start += data[i].sign ? 1 : 0;
        counter += scalar_to_chars(data[i].mantissa, data[i].exponent, start)
                 + (data[i].sign ? 1 : 0);
      }
    };
    counter = 0;
    scalar();
    size_t volume_scalar = counter;
    std::print("Volume scalar: {}\n", volume_scalar);

    auto drag = [&data, &counter, &buffer]() {
      using jkj::dragonbox::detail::to_chars;
      for (size_t i = 0; i < data.size(); ++i) {
        char *start = buffer;
        buffer[0] = '-';
        start += data[i].sign ? 1 : 0;
        counter += (to_chars(data[i].mantissa, data[i].exponent, start) - buffer)
                 + (data[i].sign ? 1 : 0);
      }
    };
    counter = 0;
    drag();
    size_t volume_drag = counter;
    std::print("Volume drag: {}\n", volume_drag);
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
    run_and_report("avx-512+champagne_lemire", avx512l, volume512);
#endif
    run_and_report("dragonbox", drag, volume_drag);
    run_and_report("scalar", scalar, volume_scalar);
  } else if constexpr (std::is_same_v<T, uint64_t>) {
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
    auto avx512l = [&data, &counter, &buffer, &algo_variant]() {
      if (algo_variant == Variant::Auto)
        algo_variant = detect_variant(data);
      if (algo_variant == Variant::Homogeneous) {
        for (size_t i = 0; i < data.size(); ++i)
          counter += avx512_to_chars<Variant::Homogeneous>(data[i], buffer);
      } else {
        for (size_t i = 0; i < data.size(); ++i)
          counter += avx512_to_chars<Variant::Heterogeneous>(data[i], buffer);
      }
    };
    counter = 0;
    Variant detected = detect_variant(data);
    std::print("Auto variant would select: {}\n",
        detected == Variant::Homogeneous ? "Homogeneous" : "Heterogeneous");
    avx512l();
    size_t volume512 = counter;
    std::print("Volume 512: {}\n", volume512);
#endif

    auto standard_to_chars = [&]() {
      for (size_t i = 0; i < data.size(); ++i)
        counter += std::to_chars(buffer, buffer + sizeof(buffer), data[i]).ptr - buffer;
    };
    counter = 0;
    standard_to_chars();
    size_t volume_standard = counter;
    std::print("Volume std::to_chars: {}\n", volume_standard);

    auto mathisen_sse = [&]() {
      for (size_t i = 0; i < data.size(); ++i)
        counter += baselines_int::mathisen_sse(data[i], buffer);
    };
    counter = 0;
    mathisen_sse();
    size_t volume_mathisen = counter;
    std::print("Volume mathisen_sse: {}\n", volume_mathisen);

    auto mula_sse64 = [&]() {
      for (size_t i = 0; i < data.size(); ++i)
        counter += baselines_int::mula_sse64(data[i], buffer);
    };
    counter = 0;
    mula_sse64();
    size_t volume_mula_sse64 = counter;
    std::print("Volume mula_sse64: {}\n", volume_mula_sse64);

    auto hopman = [&]() {
      for (size_t i = 0; i < data.size(); ++i)
        counter += baselines_int::hopman_fast(data[i], buffer);
    };
    counter = 0;
    hopman();
    size_t volume_hopman_fast = counter;
    std::print("Volume hopman_fast: {}\n", volume_hopman_fast);

    auto naive_onepass = [&]() {
      for (size_t i = 0; i < data.size(); ++i)
        counter += baselines_int::naive(data[i], buffer);
    };
    counter = 0;
    naive_onepass();
    size_t volume_naive_onepass = counter;
    std::print("Volume naive_onepass: {}\n", volume_naive_onepass);

#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
    run_and_report("avx-512+champagne_lemire", avx512l, volume512);
#endif
    run_and_report("std::to_chars", standard_to_chars, volume_standard);
    run_and_report("mathisen_sse_u64", mathisen_sse, volume_mathisen);
    run_and_report("mula_sse64", mula_sse64, volume_mula_sse64);
    run_and_report("hopman_fast", hopman, volume_hopman_fast);
    run_and_report("naive_onepass", naive_onepass, volume_naive_onepass);
  } else {
    static_assert(false, "Unsupported type");
  }
}

int main(int argc, char **argv) {
#ifdef __clang__
  std::print("Clang version: {}.{}.{}\n", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
  std::print("GCC version: {}.{}.{}\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif
  std::print("==============================================================\n");
  std::print("Warning: the results are sensitive to the compiler being used.\n");
  std::print("Try with GCC and LLVM.\n");
  std::print("==============================================================\n");

#ifndef __OPTIMIZE__
  std::print("Warning: Build is not optimized. Performance may be poor.\n");
#endif

  cxxopts::Options options("benchmark", "Float to string conversion benchmark");

  options.add_options()
    ("h,help",  "Show help message")
    ("i,int",   "Evaluate integer conversion (uint64_t) instead of floats")
    ("q,quick", "Do a quick validation test with some hardcoded cases")
    ("f,file",  "Input file containing numbers", cxxopts::value<std::string>())
    ("n,num",   "Number of random numbers to generate", cxxopts::value<size_t>()->default_value("1000000"))
    ("m,min",   "Minimum mantissa digits for random generation (1-17)", cxxopts::value<int>()->default_value("1"))
    ("M,max",   "Maximum mantissa digits for random generation (1-17)", cxxopts::value<int>()->default_value("17"))
    ("v,variant", "Variant of our AVX512 algorithm to benchmark:"
                  "'homo', 'hetero' or 'auto'", cxxopts::value<std::string>()->default_value("auto"))
    ("d,distribution", "Distribution mode: 'uniform' (equal probability for each digit count)"
                       "or 'natural' (more high-digit numbers)", cxxopts::value<std::string>()->default_value("natural"));

  std::variant<std::vector<decimal_float>, std::vector<uint64_t>> data;
  Variant algo_variant;
  try {
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::print("{}\n", options.help());
      std::print("\nExamples:\n");
      std::print("  {} -n 1000             # Random 1000 numbers with 1-17 digits precision\n", argv[0]);
      std::print("  {} -f data/canada.txt  # Use data from file\n", argv[0]);
      std::print("  {} -v homo             # Use the variant optimized for homogeneous digits length\n", argv[0]);
      std::print("  {} -m 1 -M 20 -i       # Random uint64_t with 1-20 digits (uniform)\n", argv[0]);
      std::print("  {} -m 10 -M 17         # Random floats with 10-17 digit mantissas (uniform)\n", argv[0]);
      std::print("  {} -m 5 -M 15 -d natural  # Natural distribution with 5-15 digit range\n", argv[0]);
      return EXIT_SUCCESS;
    }

    const bool integer_mode = result.count("int") > 0;
    if (result.count("quick")) {
      bool success = integer_mode
                   ? test_some_harcoded_integers()
                   : test_some_harcoded_floats();
      if(!success) {
        std::print("Some tests failed!\n");
        if(integer_mode)
          return EXIT_FAILURE;
        //
        std::print("Failures with floats are expected!\n");
        return EXIT_SUCCESS;

      }
      std::print("All tests passed!\n");
      return EXIT_SUCCESS;
    }

    // Parse distribution mode
    const std::string variant_str = result["variant"].as<std::string>();
    if (variant_str == "auto") {
      algo_variant = Variant::Auto;
    } else if (variant_str == "homo") {
      algo_variant = Variant::Homogeneous;
    } else if (variant_str == "hetero") {
      algo_variant = Variant::Heterogeneous;
    } else {
      std::print(stderr, "Error: variant must be 'homo', 'hetero' or 'auto'\n");
      return EXIT_FAILURE;
    }

    // Validate digit ranges
    const int min_digits = result["min"].as<int>();
    const int max_digits = result["max"].as<int>();
    const int max_allowed = integer_mode ? 20 : 17;
    if ((min_digits < 1) | (min_digits > max_allowed) |
        (min_digits < 1) | (max_digits > max_allowed) |
        (min_digits > max_digits)) {
      std::print(stderr, "Error: invalid digit range [{}, {}]\n", min_digits, max_digits);
      return EXIT_FAILURE;
    }

    if (result.count("file")) { // Load data from file
      const std::string filename = result["file"].as<std::string>();

      if (integer_mode) {
        const auto ints = read_from_file<uint64_t>(filename);
        if (ints.empty()) {
          std::print(stderr, "No valid integers found in the file: {}\n", filename);
          return EXIT_FAILURE;
        }
        std::print("Loaded {} integers from file: {}\n", ints.size(), filename);
        data = std::move(ints);
      } else {
        const auto floats = read_from_file<double>(filename);
        if (floats.empty()) {
          std::print(stderr, "No valid floats found in the file: {}\n", filename);
          return EXIT_FAILURE;
        }
        std::print("Loaded {} floats from file: {}\n", floats.size(), filename);

        std::vector<decimal_float> floats_as_decimals;
        floats_as_decimals.reserve(floats.size());
        for (double f : floats)
          floats_as_decimals.push_back(double_to_decimal_float(f));
        data = std::move(floats_as_decimals);
      }
    } else { // Generate random data
      // Parse distribution mode
      const std::string distribution_str = result["distribution"].as<std::string>();
      DistributionMode distribution_mode;
      if (distribution_str == "uniform") {
        distribution_mode = DistributionMode::Uniform;
      } else if (distribution_str == "natural") {
        distribution_mode = DistributionMode::Natural;
      } else {
        std::print(stderr, "Error: distribution must be 'uniform' or 'natural'\n");
        return EXIT_FAILURE;
      }

      const size_t num_values = result["num"].as<size_t>();
      if (integer_mode)
        data = generate_large_set<uint64_t>(num_values, min_digits, max_digits, distribution_mode);
      else
        data = generate_large_set<decimal_float>(num_values, min_digits, max_digits, distribution_mode);
      std::print("Generated {} random {} with digits in range [{}, {}] using {} distribution\n",
                 num_values, integer_mode ? "integers" : "floats",
                 min_digits, max_digits, distribution_str);
    }
  } catch (const cxxopts::exceptions::exception& e) {
    std::print(stderr, "Error parsing arguments: {}\n", e.what());
    std::print(stderr, "Use -h or --help for usage information.\n");
    return EXIT_FAILURE;
  }

  // Print length distribution
  std::visit([](auto &vec) {
    std::array<size_t, 21> lengthDistrib{};
    for (const auto &v : vec) {
      uint64_t number = [&] {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, decimal_float>)
          return v.mantissa;
        else
          return v;
      }();
      ++lengthDistrib[fast_digit_count(number)];
    }

    std::print("length distribution:\n");
    for (size_t i = 1; i < lengthDistrib.size(); ++i)
      std::print("\t{:2}: {}\n", i, lengthDistrib[i]);
  }, data);

  std::visit([av = algo_variant](auto &vec) {
      run_benchmark(vec, av);
  }, data);
}
