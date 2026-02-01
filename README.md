# Integer-to-String Benchmark

A benchmark suite for integer-to-string conversion algorithms,
containing our `Champagne-Lemire` AVX-512 algorithm.

## Overview

This project benchmarks various algorithms for converting 64-bit unsigned
integers to their decimal string representation. The focus is on high-throughput
batch conversion scenarios.

## Requirements

- An x86 64-bit CPU
- AVX-512 support (for the `Champagne-Lemire` algorithm)
- A recent GCC or Clang on a Linux system
- CMake

## Building

```bash
cmake -B build
cmake --build build
```

With a specific compiler, e.g., clang:

```bash
CC=clang CXX=clang++ cmake -B buildclang
cmake --build buildclang
```

## Usage

Run the benchmark:

```bash
./build/benchmark
```

Quick validation mode:

```bash
./build/benchmark -q
```

With input data files:

```bash
./build/benchmark -f data/citm_catalog_integers.txt
./build/benchmark -f data/twitterjson_integers.txt
```

Performance counters may require privileged execution (`sudo`).

## Algorithms Benchmarked

- **Champagne-Lemire (homogeneous)**: AVX-512 algorithm; variant optimized for uniform digit counts
- **Champagne-Lemire (heterogeneous)**: AVX-512 algorithm; variant for varying digit counts
- **Champagne-Lemire (auto)**: Dynamically selects between homogeneous/heterogeneous variants
- **std::to_chars**: Standard library implementation
- **Abseil**: Google's `FastIntToBuffer` routine
- **jeaiii**: James Edward Anhalt III's algorithm
- **AppNexus**: From the AppNexus Common Framework library
- **yy_itoa**: Yao Yuan's implementation
- **Mathisen**: SSE4.1-based algorithm inspired by Mathisen's arithmetic approach
- **Muła**: Wojciech Muła's SSE-based algorithm
- **Hopman**: The `hopman_fast` algorithm (extended to 64-bit)
- **naive_onepass**: Classic algorithm with one-pass optimization

## Key Techniques

### Digit Counting

Fast algorithms to determine the number of decimal digits in a 64-bit integer:

- Daniel Lemire, "Counting the digits of 64-bit integers," January 2025, https://lemire.me/blog/2025/01/07/counting-the-digits-of-64-bit-integers/

### Lookup Tables

Precomputed strings for digit pairs (`00` through `99`) reduce store operations by processing two digits at a time.

### Division by Constants

Efficient computation of quotient and remainder using multiplication:

- Daniel Lemire et al., "Integer Division by Constants: Optimal Bounds," Heliyon 7(6), 2021, https://arxiv.org/abs/2012.12369

### Fixed-Digit Representations

- Daniel Lemire, "Converting integers to fix-digit representations quickly," November 2021, https://lemire.me/blog/2021/11/18/converting-integers-to-fix-digit-representations-quickly/

## Datasets

See `data/DATASETS.md` for descriptions of the included integer datasets:

- `citm_catalog_integers.txt` - Event catalog IDs (mostly 9-digit, homogeneous)
- `twitterjson_integers.txt` - Twitter API integers (heterogeneous distribution)
- `cit_patents_citing_integers.txt.gz` - US patent numbers (7-digit, homogeneous)
- `stackoverflow_unix_timestamps_integers.txt.gz` - Unix timestamps (10-digit, homogeneous)
