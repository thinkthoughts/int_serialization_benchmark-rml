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
#include <charconv>
#include <unordered_map>
using std::literals::string_literals::operator""s;
#include "champagne_lemire.h"

#include <vector>

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

std::vector<uint64_t> generate_large_set(size_t count = 1000'000) {
  std::vector<uint64_t> result;
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<uint64_t> dis(0, 9999999999999999ULL);
  result.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    result.push_back(dis(gen));
  }
  return result;
}

std::vector<uint64_t> read_uint64_from_file(const std::string &filename) {
  std::vector<uint64_t> values;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    uint64_t val;
    if (iss >> val) {
      values.push_back(val);
    }
  }
  return values;
}


int main(int argc, char **argv) {
  std::vector<uint64_t> data;
  if (argc > 1) {
    // Lecture du fichier passé en argument
    data = read_uint64_from_file(argv[1]);
    if (data.empty()) {
      fmt::print(stderr, "No valid uint64_t found in the file.\n");
      return EXIT_FAILURE;
    }
  } else {
    // Génération aléatoire par défaut
    data = generate_large_set();
  }
  fmt::print("Data size: {} integers\n", data.size());

  volatile uint64_t counter = 0;
  char buffer[128];
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  auto avx512l = [&data, &counter, &buffer]() {
    for (size_t i = 0; i < data.size(); ++i) {
      counter = counter +
                avx512_to_chars(data[i], buffer);
    }
  };
  counter = 0;
  avx512l();
  size_t volume512 = counter;
  fmt::print("Volume 512: {}\n", volume512);
#endif 
  auto standard_to_chars = [&data, &counter, &buffer]() {
                   for (size_t i = 0; i < data.size(); ++i) {
                     counter = counter +
                               (std::to_chars(buffer, buffer + 128, data[i]).ptr -
                                buffer);
                   }
                 };
  counter = 0;
  standard_to_chars();
  size_t volume_standard = counter;
  fmt::print("Volume standard_to_chars: {}\n", volume_standard);

  for (size_t i = 0; i < 4; i++) {
    fmt::print("Run {}\n", i + 1);
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
    pretty_print(data.size(), volume512, "avx-512+champagne_lemire", bench(avx512l));
#endif

    pretty_print(data.size(), volume_standard, "to_chars",
                 bench(standard_to_chars));
  }
#if defined(CHAMPAGNE_LEMIRE_AVX512) && CHAMPAGNE_LEMIRE_AVX512
  fmt::print("Using AVX512IFMA\n");
#else
  fmt::print("Using fallback implementation (AVX-512 not found)\n");
#endif
}
