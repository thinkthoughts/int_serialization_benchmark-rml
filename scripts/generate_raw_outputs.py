#!/usr/bin/env python3
"""
Generate raw benchmark outputs for homogeneous vs heterogeneous variant comparison.

This script compiles the benchmark code with different compilers and runs benchmarks
with various configurations, saving the raw output to files for later analysis.
"""

import subprocess
import os
import sys
import argparse
import shutil
import gzip
import tempfile
from utils import get_cpu_model

# Input data files to test (comment out to disable)
input_files = [
    'data/citm_catalog_integers.txt',
    'data/twitterjson_integers.txt',
    'data/stackoverflow_unix_timestamps_integers.txt',
    'data/cit_patents_citing_integers.txt',
]

# Generate all 20 digit-length datasets for figure (1-digit through 20-digit)
synthetic_models = []
for digit_len in range(1, 21):
    # Format: (name, description, args_list)
    synthetic_models.append((
        f'uniform-{digit_len}digit-1M',
        f'Uniform {digit_len}-digit distribution (1M samples)',
        ['-n', '1000000', '-d', 'uniform', '-m', str(digit_len), '-M', str(digit_len)]
    ))

synthetic_models.extend([
    ('uniform-1to20-1M', 'Uniform 1-20 digits distribution (1M samples)',
     ['-n', '1000000', '-d', 'uniform']),
    ('natural-8-1M', 'Natural distribution 1-8 digits (1M samples)',
     ['-n', '1000000', '-d', 'natural', '-m', '1', '-M', '8']),
    ('natural-16-1M', 'Natural distribution 1-16 digits (1M samples)',
     ['-n', '1000000', '-d', 'natural', '-m', '1', '-M', '16']),
])

# Variant flags to test (comment out to disable)
variants = [
    'homo',
    'hetero',
]

# ====================
#  END CONFIGURATION
# ====================

# Dataset categories for selective generation
# TABLE_DATASETS: used for variant comparison table (homogeneous vs heterogeneous)
# FIGURE_DATASETS: used for digit-length performance figure
TABLE_DATASETS = {
    'citm_catalog_integers',
    'twitterjson_integers',
    'stackoverflow_unix_timestamps_integers',
    'cit_patents_citing_integers',
    'natural-8-1M',
}

FIGURE_DATASETS = {f'uniform-{d}digit-1M' for d in range(1, 21)}


def should_generate_dataset(dataset_base_name, args):
    """Determine if dataset should be generated based on flags."""
    if not args.table and not args.figure:
        return True  # No flags = generate all (backward compatible)
    if args.table and dataset_base_name in TABLE_DATASETS:
        return True
    if args.figure and dataset_base_name in FIGURE_DATASETS:
        return True
    return False


def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Compile and run benchmarks with various configurations")
    parser.add_argument(
        "compiler",
        nargs='?',
        default="g++",
        help="Compiler to use (g++, clang++, or 'all' for both)")
    parser.add_argument("--build-dir",
                        default="build",
                        help="Build directory (default: build)")
    parser.add_argument(
        "--output-dir",
        default="./outputs",
        help="Output directory for raw outputs (default: ./outputs)")
    parser.add_argument("--clean",
                        action="store_true",
                        help="Clean build directory before compilation")
    parser.add_argument("--skip-compile",
                        action="store_true",
                        help="Skip compilation step (use existing build)")
    parser.add_argument(
        "--timeout",
        type=int,
        default=600,
        help="Timeout per benchmark run in seconds (default: 600)")
    parser.add_argument("--table",
                        action="store_true",
                        help="Generate only datasets needed for variant comparison table (homogeneous vs heterogeneous)")
    parser.add_argument("--figure",
                        action="store_true",
                        help="Generate only datasets needed for digit-length figure")
    return parser.parse_args()


def compile_benchmarks(compiler, build_dir, clean=False):
    """Compile the benchmark code with the specified compiler."""
    print(f"Compiling benchmarks with {compiler}...")

    # Check if we need to clean due to compiler change
    cmake_cache = os.path.join(build_dir, "CMakeCache.txt")
    if os.path.exists(cmake_cache):
        with open(cmake_cache, 'r') as f:
            cache_content = f.read()
        # Detect current compiler in cache
        if compiler == "g++" and "clang++" in cache_content:
            print(f"Compiler change detected (clang++ -> g++), cleaning build directory")
            clean = True
        elif compiler == "clang++" and "/g++" in cache_content and "clang" not in cache_content:
            print(f"Compiler change detected (g++ -> clang++), cleaning build directory")
            clean = True

    # Clean build directory if requested or needed
    if clean and os.path.exists(build_dir):
        print(f"Cleaning build directory: {build_dir}")
        shutil.rmtree(build_dir)

    # Set environment variables for compiler
    env = os.environ.copy()
    if compiler == "g++":
        env["CC"] = "gcc"
        env["CXX"] = "g++"
    elif compiler == "clang++":
        env["CC"] = "clang"
        env["CXX"] = "clang++"

    # Configure with CMake
    cmake_cmd = ["cmake", "-B", build_dir, "."]
    print(f"Running: {' '.join(cmake_cmd)}")
    subprocess.run(cmake_cmd, env=env, check=True)

    # Build with CMake
    build_cmd = ["cmake", "--build", build_dir]
    print(f"Running: {' '.join(build_cmd)}")
    subprocess.run(build_cmd, env=env, check=True)

    print("Compilation successful!\n")


def run_benchmark(benchmark_executable, output_dir, cpu_model, compiler_label,
                  label, cmd_args, timeout):
    """Run a single benchmark and save output to file."""
    # Build output file name
    safe_label = label.replace('.', '_').replace('/', '-')
    filename_raw = f"{cpu_model}_{compiler_label}_{safe_label}.raw"
    out_path_raw = os.path.join(output_dir, filename_raw)

    # Check if output file already exists
    if os.path.exists(out_path_raw):
        print(f"  → Skipping {filename_raw} (already exists)", flush=True)
        return True

    # Build command
    cmd = [benchmark_executable] + cmd_args
    print(f"Running: {' '.join(cmd)}", flush=True)

    try:
        # Run the benchmark
        result = subprocess.run(cmd,
                                capture_output=True,
                                text=True,
                                check=True,
                                timeout=timeout)

        # Save output to file
        with open(out_path_raw, 'w') as f:
            f.write(f"# Configuration: {label}\n")
            f.write(f"# Command: {' '.join(cmd)}\n")
            f.write(f"# {'=' * 70}\n\n")
            f.write(result.stdout)
            if result.stderr:
                f.write(f"\n# STDERR:\n{result.stderr}")

        print(f"  ✓ Written: {out_path_raw}\n", flush=True)
        return True

    except subprocess.TimeoutExpired:
        print(f"  ✗ TIMEOUT (exceeded {timeout} seconds)\n", flush=True)
        return False
    except subprocess.CalledProcessError as e:
        print(f"  ✗ FAILED (exit code {e.returncode})", flush=True)
        print(f"    stdout: {e.stdout[:200]}", flush=True)
        print(f"    stderr: {e.stderr[:200]}\n", flush=True)
        return False
    except Exception as e:
        print(f"  ✗ ERROR: {e}\n", flush=True)
        return False


def decompress_if_needed(filepath):
    """
    Decompress a file if it's gzipped, return the path to use.
    Returns (path_to_use, temp_file_object or None).
    Caller must clean up temp_file_object if not None.
    """
    # Check if file exists as-is
    if os.path.exists(filepath):
        return filepath, None

    # Check if .gz version exists
    gz_path = filepath + '.gz'
    if not os.path.exists(gz_path):
        return filepath, None  # Return original path, let benchmark fail

    # Decompress to temporary file
    print(f"  Decompressing {gz_path}...", flush=True)
    temp_file = tempfile.NamedTemporaryFile(mode='w',
                                            suffix='.txt',
                                            delete=False)
    with gzip.open(gz_path, 'rt') as gz_file:
        temp_file.write(gz_file.read())
    temp_file.close()

    return temp_file.name, temp_file


def run_all_benchmarks(compiler, build_dir, output_dir, timeout, args):
    """Run all configured benchmarks for a given compiler."""
    # Set up paths
    benchmark_executable = f'./{build_dir}/benchmark'
    os.makedirs(output_dir, exist_ok=True)

    # Check if benchmark executable exists
    if not os.path.exists(benchmark_executable):
        print(f"ERROR: Benchmark executable not found: {benchmark_executable}")
        print("Please build the project first or remove --skip-compile flag")
        return 0, 0

    # Get CPU model and clean it for filenames
    cpu_model = get_cpu_model().replace(' ', '_').replace('/',
                                                          '-').replace('@', '')

    # Save compiler information
    compiler_info_path = os.path.join(output_dir, f"{compiler}.txt")
    try:
        compiler_version = subprocess.check_output([compiler, "--version"],
                                                   text=True)
        with open(compiler_info_path, 'w') as f:
            f.write(compiler_version)
        print(f"Saved compiler info to: {compiler_info_path}\n")
    except Exception as e:
        print(f"Warning: Could not get compiler version: {e}\n")

    success_count = 0
    total_count = 0

    # File-based benchmarks
    for filepath in input_files:
        file_label = os.path.splitext(os.path.basename(filepath))[0]

        # Check if we should generate this dataset
        if not should_generate_dataset(file_label, args):
            print(f"Skipping {file_label} (not requested by flags)\n", flush=True)
            continue

        # Decompress once for all variants of this file
        actual_path, temp_file = decompress_if_needed(filepath)

        try:
            for variant in variants:
                label = f"{file_label}-{variant}"
                cmd_args = ['-f', actual_path, '-v', variant]
                total_count += 1
                if run_benchmark(benchmark_executable=benchmark_executable,
                                 output_dir=output_dir,
                                 cpu_model=cpu_model,
                                 compiler_label=compiler,
                                 label=label,
                                 cmd_args=cmd_args,
                                 timeout=timeout):
                    success_count += 1
        finally:
            # Clean up temporary file after all variants
            if temp_file and os.path.exists(temp_file.name):
                os.unlink(temp_file.name)

    # Synthetic model benchmarks
    for model_name, model_desc, model_args in synthetic_models:
        # Check if we should generate this dataset
        if not should_generate_dataset(model_name, args):
            continue  # Skip silently for synthetic (too many to print)

        for variant in variants:
            label = f"{model_name}-{variant}"
            cmd_args = model_args + ['-v', variant]
            total_count += 1
            if run_benchmark(benchmark_executable=benchmark_executable,
                             output_dir=output_dir,
                             cpu_model=cpu_model,
                             compiler_label=compiler,
                             label=label,
                             cmd_args=cmd_args,
                             timeout=timeout):
                success_count += 1

    return success_count, total_count


def main():
    """Main entry point."""
    args = parse_args()

    # Determine which compilers to use
    if args.compiler.lower() == "all":
        compilers = ["g++", "clang++"]
    else:
        compilers = [args.compiler]

    print("=" * 80)
    print("Benchmark Configuration")
    print("=" * 80)
    print(f"Compilers: {', '.join(compilers)}")
    print(f"Build directory: {args.build_dir}")
    print(f"Output directory: {args.output_dir}")
    print(f"File-based datasets: {len(input_files)}")
    print(f"Synthetic models: {len(synthetic_models)}")
    print(f"Variants: {len(variants)} (homo, hetero)")
    print(
        f"Total configs per compiler: {(len(input_files) + len(synthetic_models)) * len(variants)}"
    )
    print(f"Timeout per run: {args.timeout}s")
    print("=" * 80)
    print()

    total_success = 0
    total_runs = 0

    for compiler in compilers:
        print(f"\n{'=' * 80}")
        print(f"Processing compiler: {compiler}")
        print(f"{'=' * 80}\n")

        # Compile benchmarks unless skipped
        if not args.skip_compile:
            try:
                compile_benchmarks(compiler=compiler,
                                   build_dir=args.build_dir,
                                   clean=args.clean)
            except subprocess.CalledProcessError as e:
                print(f"ERROR: Compilation failed for {compiler}")
                print(f"  {e}")
                continue
            except Exception as e:
                print(f"ERROR: Unexpected error during compilation: {e}")
                continue

        # Run all benchmarks
        success, total = run_all_benchmarks(compiler=compiler,
                                            build_dir=args.build_dir,
                                            output_dir=args.output_dir,
                                            timeout=args.timeout,
                                            args=args)

        total_success += success
        total_runs += total

        print(f"\n{compiler}: {success}/{total} successful runs")

    # Final summary
    print("\n" + "=" * 80)
    print("FINAL SUMMARY")
    print("=" * 80)
    print(f"Total successful runs: {total_success}/{total_runs}")

    if total_success < total_runs:
        print(f"WARNING: {total_runs - total_success} runs failed")
        sys.exit(1)
    else:
        print("All benchmarks completed successfully!")
        print(f"\nRaw outputs saved in: {args.output_dir}/")
        print("Next step: Run generate_table.py to create the LaTeX table")


if __name__ == "__main__":
    main()
