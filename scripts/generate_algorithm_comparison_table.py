#!/usr/bin/env python3
"""
Parse raw benchmark outputs and generate LaTeX table comparing all algorithms.

This script reads benchmark output files and creates a comprehensive comparison
table showing all algorithms across multiple datasets with key performance metrics.
"""

import re
from pathlib import Path
from typing import Optional, Dict, List, Tuple
import argparse
from utils import get_cpu_model
import sys

# Algorithms to include in the comparison
# Format: (pattern_in_output, display_name)
# Ordered: ours first, most competitive next, then by approximate performance
ALGORITHMS = [
    ('avx-512\\+champagne_lemire', 'Champagne--Lemire'),
    ('jeaiii_fast_uint64', 'jeaiii'),
    ('itoa_yy_64', 'yy'),
    ('itoa_an_64', 'AppNexus'),
    ('absl_fastint', 'Abseil FastInt'),
    ('std::to_chars', 'std::to\\_chars'),
    ('mula_sse64', 'Mula SSE64'),
    ('mathisen_sse_u64', 'Mathisen SSE'),
    ('hopman_fast', 'Hopman'),
    ('naive_onepass', 'Naive'),
]

# Dataset groups to include in the table
# Format: (base_name, display_name)
DATASETS = [
    # ("citm_catalog_integers", "CITM Catalog"),
    ("twitterjson_integers", "Twitter JSON"),
    ("cit_patents_citing_integers", "CIT Patents"),
    # ("stackoverflow_unix_timestamps_integers", "Stack Overflow"),
    ("natural-8-1M", "Natural 1-8 digit"),
]

# Metrics to extract
# Format: (pattern_suffix, column_header, latex_unit)
METRICS = [
    ('ns/d', 'ns/d', 'ns/d'),  # nanoseconds per digit (character)
    ('i/d', 'i/d', 'ins/d'),   # instructions per digit
    ('c/d', 'c/d', 'cyc/d'),   # cycles per digit
]


def parse_algorithm_metrics(file_path: Path, algorithm_pattern: str) -> Optional[Dict[str, float]]:
    """
    Parse a benchmark output file and extract all metrics for a specific algorithm.

    Returns a dict with metric names as keys and values as floats.
    Returns None if parsing fails or algorithm not found.
    """
    try:
        with open(file_path, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        return None

    # Build regex pattern to capture the full line for this algorithm
    # Example: "avx-512+champagne_lemire  :   1.94 ns/d   5.48 GHz  10.64 c/d ..."
    escaped_pattern = algorithm_pattern
    pattern = rf'{escaped_pattern}\s*:\s*([\d.]+)\s*ns/d\s*[\d.]+\s*GHz\s*([\d.]+)\s*c/d\s*([\d.]+)\s*i/d'

    match = re.search(pattern, content)
    if not match:
        return None
    return {
        'ns/d': float(match.group(1)),
        'c/d': float(match.group(2)),
        'i/d': float(match.group(3)),
    }


def extract_auto_selection(file_path: Path) -> Optional[str]:
    """
    Extract the auto-selected variant from the output file.

    Returns 'Homogeneous' or 'Heterogeneous' or None.
    """
    try:
        with open(file_path, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        return None

    pattern = r'Auto variant would select:\s*(Homogeneous|Heterogeneous)'
    match = re.search(pattern, content)
    return match.group(1) if match else None


def find_dataset_files(base_name: str, compiler: str, output_dir: str) -> Tuple[Optional[Path], Optional[Path]]:
    """
    Find output files for a dataset.

    Returns (homo_file, hetero_file) paths.
    Files may not exist - returns None for missing files.
    """
    cpu_model = get_cpu_model().replace(' ', '_').replace('/', '-').replace('@', '')

    # Try with CPU model and compiler prefix
    homo_file = Path(output_dir) / f"{cpu_model}_{compiler}_{base_name}-homo.raw"
    hetero_file = Path(output_dir) / f"{cpu_model}_{compiler}_{base_name}-hetero.raw"

    # Fallback: try without CPU model prefix (backward compatibility)
    if not homo_file.exists():
        homo_file = Path(output_dir) / f"{base_name}-homo.raw"
        hetero_file = Path(output_dir) / f"{base_name}-hetero.raw"

    return (homo_file if homo_file.exists() else None,
            hetero_file if hetero_file.exists() else None)


def get_algorithm_results_for_dataset(dataset_name: str, compiler: str,
                                      output_dir: str) -> Optional[Dict[str, Dict[str, float]]]:
    """
    Get all algorithm results for a specific dataset.

    Uses the auto-selected variant (homogeneous or heterogeneous) based on
    what's reported in the benchmark output files.

    Returns dict mapping algorithm names to their metrics.
    Returns None if dataset files not found.
    """
    homo_file, hetero_file = find_dataset_files(dataset_name, compiler, output_dir)
    if homo_file is None and hetero_file is None:
        return None

    # Determine which variant was auto-selected
    selected = None
    if homo_file:
        selected = extract_auto_selection(homo_file)
    if selected is None and hetero_file:
        selected = extract_auto_selection(hetero_file)

    # Use the auto-selected variant's file, fallback to homo if no selection info
    if selected == 'Heterogeneous' and hetero_file:
        file_path = hetero_file
    elif selected == 'Homogeneous' and homo_file:
        file_path = homo_file
    elif homo_file:
        file_path = homo_file
    elif hetero_file:
        file_path = hetero_file
    else:
        return None

    results = {}
    for algo_pattern, algo_display_name in ALGORITHMS:
        metrics = parse_algorithm_metrics(file_path, algo_pattern)
        if metrics is not None:
            results[algo_display_name] = metrics
    return results if results else None


def format_metric_cell(value: Optional[float], is_best: bool = False) -> str:
    """Format a metric value for LaTeX table with 3 significant digits, bolding if best."""
    if value is None:
        return "---"

    # Format to 3 significant digits
    formatted = f"{value:.3g}"
    if is_best:
        return f"\\textbf{{{formatted}}}"
    return formatted


def generate_latex_table(compiler: str, output_dir: str) -> str:
    """Generate the complete LaTeX table with Option 1 structure."""
    lines = []

    # Collect all data first
    all_data: Dict[str, Dict[str, Dict[str, float]]] = {}  # dataset -> algo -> metrics
    missing_datasets = []

    for dataset_base, dataset_display in DATASETS:
        results = get_algorithm_results_for_dataset(dataset_base, compiler, output_dir)
        if results is None:
            missing_datasets.append(dataset_display)
            all_data[dataset_display] = {}
        else:
            all_data[dataset_display] = results

    # Get Champagne--Lemire baseline values for speedup calculation
    baseline_display = ALGORITHMS[0][1]  # "Champagne--Lemire"
    baseline_values: Dict[Tuple[str, str], float] = {}  # (dataset, metric) -> value
    for dataset_display, dataset_results in all_data.items():
        baseline_metrics = dataset_results.get(baseline_display, {})
        for metric_key, _, _ in METRICS:
            if metric_key in baseline_metrics:
                baseline_values[(dataset_display, metric_key)] = baseline_metrics[metric_key]

    # Find best values for each (dataset, metric) combination
    best_values: Dict[Tuple[str, str], float] = {}
    for dataset_display, dataset_results in all_data.items():
        for metric_key, _, _ in METRICS:
            values = [algo_data[metric_key] for algo_data in dataset_results.values()
                      if metric_key in algo_data]
            if values:
                best_values[(dataset_display, metric_key)] = min(values)

    # Table header
    lines.append(r"\begin{table}")
    lines.append(r"  \caption{Performance comparison of integer-to-string algorithms across datasets.")
    lines.append(r"  Metrics: ns/d = nanoseconds, i/d = instructions, c/d = cycles per character.")
    lines.append(r"  \textbf{Bold} indicates fastest; \% shows difference vs Champagne--Lemire (positive = slower).}%")
    lines.append(r"  \label{tab:algorithm_comparison}")
    lines.append(r"  \centering")
    lines.append(r"  \small")
    lines.append(r"  \begin{tabular}{llrrrrrr}")
    lines.append(r"    \toprule")
    lines.append(r"    Algorithm & Dataset & ns/d & \% & i/d & \% & c/d & \% \\")
    lines.append(r"    \midrule")

    # Data rows - outer loop: algorithms, inner loop: datasets
    for algo_idx, (algo_pattern, algo_display) in enumerate(ALGORITHMS):
        # Determine row color: alternate between black!10 and white
        row_color = r"\rowcolor{black!10}" if algo_idx % 2 == 0 else r"\rowcolor{white}"

        for dataset_idx, (dataset_base, dataset_display) in enumerate(DATASETS):
            dataset_results = all_data.get(dataset_display, {})
            algo_metrics = dataset_results.get(algo_display, {})

            row_parts = []
            # Show algorithm name only for first dataset of each algorithm
            if dataset_idx == 0:
                row_parts.append(algo_display)
            else:
                row_parts.append("")

            # Check if this algorithm has the best ns/d for this dataset
            value_nsd = algo_metrics.get('ns/d')
            best_nsd = best_values.get((dataset_display, 'ns/d'))
            is_best_algo = (value_nsd is not None and best_nsd is not None and
                            abs(value_nsd - best_nsd) < 0.01)

            # Bold dataset name if this is the best algorithm
            if is_best_algo:
                row_parts.append(f"\\textbf{{{dataset_display}}}")
            else:
                row_parts.append(dataset_display)

            # Add metrics without bolding
            for metric_key, _, _ in METRICS:
                value = algo_metrics.get(metric_key)
                row_parts.append(format_metric_cell(value, is_best=False))

                # Calculate speedup percentage vs Champagne--Lemire
                baseline_value = baseline_values.get((dataset_display, metric_key))
                if value is not None and baseline_value is not None and baseline_value > 0:
                    # Positive % = slower than baseline, negative % = faster than baseline
                    speedup_pct = ((value - baseline_value) / baseline_value) * 100
                    if algo_idx == 0:  # Champagne--Lemire itself
                        row_parts.append("-")  # No speedup comparison for baseline
                    else:
                        row_parts.append(f"{speedup_pct:+.1f}")
                else:
                    row_parts.append("-")

            lines.append("    " + row_color)
            lines.append("    " + " & ".join(row_parts) + r" \\")

    # Table footer
    lines.append(r"    \bottomrule")
    lines.append(r"  \end{tabular}\restartrowcolors")
    lines.append(r"\end{table}")

    # Warnings for missing data
    if missing_datasets:
        print("\nWARNING: Missing data for the following datasets:")
        for name in missing_datasets:
            print(f"  - {name}")
        print()

    return "\n".join(lines)


def get_available_compilers(output_dir: Path) -> List[str]:
    """Detect which compilers have data available."""
    compilers = []
    for compiler in ["g++", "clang++"]:
        pattern = f"*_{compiler}_*.raw"
        if list(output_dir.glob(pattern)):
            compilers.append(compiler)
    return compilers


def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Generate algorithm comparison LaTeX table from benchmark outputs")
    parser.add_argument(
        "--compiler",
        default=None,
        help="Compiler to use for finding output files. If not specified, generates tables for all available compilers.")
    parser.add_argument(
        "--input-dir",
        default="./outputs",
        help="Directory containing raw benchmark files (default: ./outputs)")
    parser.add_argument(
        "--output-file",
        default="table_algorithm_comparison",
        help="Output LaTeX file base name without extension (default: table_algorithm_comparison)")
    return parser.parse_args()


def process_compiler(compiler: str, input_dir: str, output_base: str) -> bool:
    """Process data and generate table for a single compiler."""
    print(f"Reading from: {input_dir}/")
    print(f"Compiler: {compiler}\n")

    # Generate table
    latex_table = generate_latex_table(compiler, input_dir)

    # Save to file in input directory
    output_path = Path(input_dir) / f"{output_base}_{compiler}.tex"
    with open(output_path, 'w') as f:
        f.write(latex_table)

    print(f"\nLaTeX table generated: {output_path}")
    print("\nTable preview:")
    print("=" * 80)
    print(latex_table)
    print("=" * 80)

    print("\nYou can include this table in your LaTeX document with:")
    print(f"  \\input{{{output_path}}}")
    return True


def main():
    """Main entry point."""
    args = parse_args()

    # Check if input directory exists
    if not Path(args.input_dir).is_dir():
        print(f"ERROR: Input directory not found: {args.input_dir}/")
        print("Please run benchmarks and save outputs first.")
        sys.exit(1)

    # Determine which compilers to process
    if args.compiler:
        compilers = [args.compiler]
    else:
        compilers = get_available_compilers(Path(args.input_dir))
        if not compilers:
            print("ERROR: No compiler data found in output directory.")
            print("Run benchmarks first or specify --compiler explicitly.")
            sys.exit(1)
        print(f"Auto-detected compilers with data: {', '.join(compilers)}\n")

    print("Parsing benchmark outputs for algorithm comparison...")

    success_count = 0
    for compiler in compilers:
        print(f"\n{'='*60}")
        print(f"Processing compiler: {compiler}")
        print('='*60)
        if process_compiler(compiler, args.input_dir, args.output_file):
            success_count += 1

    if success_count == 0:
        sys.exit(1)

    print(f"\nGenerated {success_count} table(s) successfully.")


if __name__ == "__main__":
    main()
