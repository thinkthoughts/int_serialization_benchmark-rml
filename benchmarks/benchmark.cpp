
#if defined(__x86_64__) || defined(_M_X64)
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <random>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <print>
#include <cxxopts.hpp>
using std::literals::string_literals::operator""s;

#include "performancecounters/benchmarker.h"
#include "champagne_lemire.h"
#include "integer_conv_baselines.h"

constexpr size_t Number_Benchmark_Runs = 1;
constexpr double Ratio_To_Sample = 0.01;
constexpr double Ratio_Homogeneous = 0.95; // Homogeneous mode if > 95% of
                                           // numbers have the same digit length

void pretty_print(size_t num_integers, size_t volume, const std::string &name,
                  event_aggregate agg) {
  std::print("{:<50} : ", name);
  std::print(" {:5.2f} ns/n ", agg.fastest_elapsed_ns() / num_integers);
  if (collector.has_events()) {
    std::print(" {:5.2f} GHz ",
               agg.fastest_cycles() / agg.fastest_elapsed_ns());
    std::print(" {:5.2f} c/n ", agg.fastest_cycles() / num_integers);
    std::print(" {:5.2f} i/n ", agg.fastest_instructions() / num_integers);
    std::print(" {:5.2f} B/n ", agg.branches() / num_integers);
    std::print(" {:5.2f} BM/n ", agg.branch_misses() / num_integers);
    std::print(" {:5.2f} i/d ", agg.fastest_instructions() / volume);
    std::print(" {:5.2f} i/c ",
               agg.fastest_instructions() / agg.fastest_cycles());
  }
  std::print("\n");
}

enum class DistributionMode {
  Uniform,  // Uniform distribution across digit lengths
  Natural   // Natural distribution (more high-digit numbers)
};

std::vector<uint64_t> generate_large_set(size_t count = 1'000'000,
                                         int min_digits = 1, int max_digits = 20,
                                         DistributionMode mode = DistributionMode::Uniform) {
  std::vector<uint64_t> result;
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

  return result;
}

std::vector<uint64_t> read_from_file(const std::string &filename) {
  std::vector<uint64_t> values;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    if (uint64_t val; iss >> val)
      values.push_back(val);
  }
  return values;
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

  n = baselines_int::absl_fastint(number, buffer);
  // buffer[n] = '\0'; // absl_fastint already null-terminates
  std::print("absl_fastint:          {}\n", buffer);

  n = baselines_int::jeaiii_fast_uint64(number, buffer);
  buffer[n] = '\0';
  std::print("jeaiii_fast_uint64:    {}\n", buffer);

  n = baselines_int::itoa_an_64(number, buffer);
  buffer[n] = '\0';
  std::print("itoa_an_64:            {}\n", buffer);

  n = baselines_int::itoa_yy_64(number, buffer);
  buffer[n] = '\0';
  std::print("itoa_yy_64:            {}\n", buffer);

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

bool test_some_hardcoded_integers() {
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

Variant detect_variant(const std::vector<uint64_t> &data) {
  const auto start = std::chrono::steady_clock::now();

  const size_t sample_size = Ratio_To_Sample * size(data);
  std::array<size_t, 21> lengthDistrib{};
  for (size_t i = 0; i < sample_size; ++i) {
    ++lengthDistrib[fast_digit_count(data[i])];
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

void run_benchmark(const std::vector<uint64_t> &data, [[maybe_unused]] Variant algo_variant = Variant::Auto) {
  uint64_t counter = 0;
  char buffer[128];

  // --- Helper to pretty-print run results ---
  auto run_and_report = [&](auto&& name, auto&& func, size_t volume) {
    std::print("\n");
    for (size_t i = 0; i < Number_Benchmark_Runs; ++i)
      pretty_print(data.size(), volume, name, bench(func));
  };

#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  auto avx512l = [&data, &counter, &buffer, &algo_variant]() {
    if (algo_variant == Variant::Auto)
      algo_variant = detect_variant(data);
    uint64_t local_counter = 0;
    if (algo_variant == Variant::Homogeneous) {
      for (size_t i = 0; i < data.size(); ++i)
        local_counter += avx512_to_chars<Variant::Homogeneous>(data[i], buffer);
    } else {
      for (size_t i = 0; i < data.size(); ++i)
        local_counter += avx512_to_chars<Variant::Heterogeneous>(data[i], buffer);
    }
    counter = local_counter;
  };
  Variant detected = detect_variant(data);
  std::print("Auto variant would select: {}\n",
      detected == Variant::Homogeneous ? "Homogeneous" : "Heterogeneous");
  avx512l();
  size_t volume512 = counter;
  std::print("Volume 512: {}\n", volume512);
#endif

  auto standard_to_chars = [&]() {
    uint64_t local_counter = 0;
    for (size_t i = 0; i < data.size(); ++i)
      local_counter += std::to_chars(buffer, buffer + sizeof(buffer), data[i]).ptr - buffer;
    counter = local_counter;
  };
  standard_to_chars();
  size_t volume_standard = counter;
  std::print("Volume std::to_chars: {}\n", volume_standard);

  auto absl_fastint = [&]() {
    uint64_t local_counter = 0;
    for (size_t i = 0; i < data.size(); ++i)
      local_counter += baselines_int::absl_fastint(data[i], buffer);
    counter = local_counter;
  };
  absl_fastint();
  size_t volume_absl_fastint = counter;
  std::print("Volume absl_fastint: {}\n", volume_absl_fastint);

  auto jeaiii_fast = [&]() {
    uint64_t local_counter = 0;
    for (size_t i = 0; i < data.size(); ++i)
      local_counter += baselines_int::jeaiii_fast_uint64(data[i], buffer);
    counter = local_counter;
  };
  jeaiii_fast();
  size_t volume_jeaiii_fast = counter;
  std::print("Volume jeaiii_fast_uint64: {}\n", volume_jeaiii_fast);

  auto itoa_an_64 = [&]() {
    uint64_t local_counter = 0;
    for (size_t i = 0; i < data.size(); ++i)
      local_counter += baselines_int::itoa_an_64(data[i], buffer);
    counter = local_counter;
  };
  itoa_an_64();
  size_t volume_itoa_an_64 = counter;
  std::print("Volume itoa_an_64: {}\n", volume_itoa_an_64);

  auto itoa_yy_64 = [&]() {
    uint64_t local_counter = 0;
    for (size_t i = 0; i < data.size(); ++i)
      local_counter += baselines_int::itoa_yy_64(data[i], buffer);
    counter = local_counter;
  };
  itoa_yy_64();
  size_t volume_itoa_yy_64 = counter;
  std::print("Volume itoa_yy_64: {}\n", volume_itoa_yy_64);
  auto mathisen_sse = [&]() {
    uint64_t local_counter = 0;
    for (size_t i = 0; i < data.size(); ++i)
      local_counter += baselines_int::mathisen_sse(data[i], buffer);
    counter = local_counter;
  };
  mathisen_sse();
  size_t volume_mathisen = counter;
  std::print("Volume mathisen_sse: {}\n", volume_mathisen);
  auto mula_sse64 = [&]() {
    uint64_t local_counter = 0;
    for (size_t i = 0; i < data.size(); ++i)
      local_counter += baselines_int::mula_sse64(data[i], buffer);
    counter = local_counter;
  };
  mula_sse64();
  size_t volume_mula_sse64 = counter;
  std::print("Volume mula_sse64: {}\n", volume_mula_sse64);

  auto hopman = [&]() {
    uint64_t local_counter = 0;
    for (size_t i = 0; i < data.size(); ++i)
      local_counter += baselines_int::hopman_fast(data[i], buffer);
    counter = local_counter;
  };
  hopman();
  size_t volume_hopman_fast = counter;
  std::print("Volume hopman_fast: {}\n", volume_hopman_fast);

  auto naive_onepass = [&]() {
    uint64_t local_counter = 0;
    for (size_t i = 0; i < data.size(); ++i)
      local_counter += baselines_int::naive(data[i], buffer);
    counter = local_counter;
  };
  naive_onepass();
  size_t volume_naive_onepass = counter;
  std::print("Volume naive_onepass: {}\n", volume_naive_onepass);

#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  run_and_report("avx-512+champagne_lemire", avx512l, volume512);
#endif
  run_and_report("std::to_chars", standard_to_chars, volume_standard);
  run_and_report("absl_fastint", absl_fastint, volume_absl_fastint);
  run_and_report("jeaiii_fast_uint64", jeaiii_fast, volume_jeaiii_fast);
  run_and_report("itoa_an_64", itoa_an_64, volume_itoa_an_64);
  run_and_report("itoa_yy_64", itoa_yy_64, volume_itoa_yy_64);
  run_and_report("mathisen_sse_u64", mathisen_sse, volume_mathisen);
  run_and_report("mula_sse64", mula_sse64, volume_mula_sse64);
  run_and_report("hopman_fast", hopman, volume_hopman_fast);
  run_and_report("naive_onepass", naive_onepass, volume_naive_onepass);
}

int main(int argc, char **argv) {
  cxxopts::Options options("benchmark", "Integer to string conversion benchmark");

  options.add_options()
    ("h,help",  "Show help message")
    ("q,quick", "Do a quick validation test with some hardcoded cases")
    ("f,file",  "Input file containing integers", cxxopts::value<std::string>())
    ("n,num",   "Number of random integers to generate", cxxopts::value<size_t>()->default_value("1000000"))
    ("m,min",   "Minimum digits for random generation (1-20)", cxxopts::value<int>()->default_value("1"))
    ("M,max",   "Maximum digits for random generation (1-20)", cxxopts::value<int>()->default_value("20"))
    ("v,variant", "Variant of our AVX512 algorithm to benchmark:"
                  "'homo', 'hetero' or 'auto'", cxxopts::value<std::string>()->default_value("auto"))
    ("d,distribution", "Distribution mode: 'uniform' (equal probability for each digit count)"
                       "or 'natural' (more high-digit numbers)", cxxopts::value<std::string>()->default_value("natural"));

  std::vector<uint64_t> data;
  Variant algo_variant;
  try {
    auto result = options.parse(argc, argv);

    // Check for unexpected positional arguments
    if (!result.unmatched().empty()) {
      std::print(stderr, "Error: unexpected argument '{}'\n", result.unmatched()[0]);
      std::print(stderr, "Use -f <file> to specify an input file.\n");
      std::print(stderr, "Run with --help for usage information.\n");
      return EXIT_FAILURE;
    }

    if (result.count("help")) {
      std::print("{}\n", options.help());
      std::print("\nExamples:\n");
      std::print("  {} -n 1000                        # Random 1000 integers with 1-20 digits\n", argv[0]);
      std::print("  {} -f data/twitterjson_integers.txt  # Use data from file\n", argv[0]);
      std::print("  {} -v homo                        # Use the variant optimized for homogeneous digit lengths\n", argv[0]);
      std::print("  {} -m 1 -M 20                     # Random uint64_t with 1-20 digits\n", argv[0]);
      std::print("  {} -m 10 -M 15                    # Random integers with 10-15 digits\n", argv[0]);
      std::print("  {} -m 5 -M 15 -d natural          # Natural distribution with 5-15 digit range\n", argv[0]);
      return EXIT_SUCCESS;
    }

    // Print compiler/build info banner
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

    if (result.count("quick")) {
      bool success = test_some_hardcoded_integers();
      if(!success) {
        std::print("Some tests failed!\n");
        return EXIT_FAILURE;
      }
      std::print("All tests passed!\n");
      return EXIT_SUCCESS;
    }

    // Parse variant
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
    if ((min_digits < 1) | (min_digits > 20) |
        (max_digits < 1) | (max_digits > 20) |
        (min_digits > max_digits)) {
      std::print(stderr, "Error: invalid digit range [{}, {}]\n", min_digits, max_digits);
      return EXIT_FAILURE;
    }

    if (result.count("file")) { // Load data from file
      const std::string filename = result["file"].as<std::string>();

      data = read_from_file(filename);
      if (data.empty()) {
        std::print(stderr, "No valid integers found in the file: {}\n", filename);
        return EXIT_FAILURE;
      }
      std::print("Loaded {} integers from file: {}\n", data.size(), filename);
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
      data = generate_large_set(num_values, min_digits, max_digits, distribution_mode);
      std::print("Generated {} random integers with digits in range [{}, {}] using {} distribution\n",
                 num_values, min_digits, max_digits, distribution_str);
    }
  } catch (const cxxopts::exceptions::exception& e) {
    std::print(stderr, "Error parsing arguments: {}\n", e.what());
    std::print(stderr, "Use -h or --help for usage information.\n");
    return EXIT_FAILURE;
  }

  // Print length distribution
  std::array<size_t, 21> lengthDistrib{};
  for (const auto &v : data) {
    ++lengthDistrib[fast_digit_count(v)];
  }

  std::print("length distribution:\n");
  for (size_t i = 1; i < lengthDistrib.size(); ++i)
    std::print("\t{:2}: {}\n", i, lengthDistrib[i]);

  run_benchmark(data, algo_variant);
}

#else // defined(__x86_64__) || defined(_M_X64)
#include <cstdlib>
#include <print>
int main() {
  std::print("This benchmark requires an x86-64 architecture.\n");
  return EXIT_FAILURE;
}
#endif // defined(__x86_64__) || defined(_M_X64)
