#!/usr/bin/env python3
"""
Generate performance comparison figures for fixed-length digit datasets.
Similar to the itoa-u64-fixed-length.png figure in ibireme/c_numconv_benchmark.
"""

import sys
import re
import argparse
from pathlib import Path
from typing import Dict, List, Optional, Tuple
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend

# Algorithm list - same as table script
ALGORITHMS = [
    ('avx-512\\+champagne_lemire', 'Champagne--Lemire'),
    ('jeaiii_fast_uint64', 'jeaiii'),
    ('itoa_yy_64', 'yy'),
    ('itoa_an_64', 'AppNexus'),
    ('absl_fastint', 'Abseil FastInt'),
    ('std::to_chars', 'std::to_chars'),
    ('mula_sse64', 'Mula SSE64'),
    ('mathisen_sse_u64', 'Mathisen SSE'),
    ('hopman_fast', 'Hopman'),
    ('naive_onepass', 'Naive'),
]

# Simplified algorithm list for main paper (5 key algorithms)
ALGORITHMS_SIMPLIFIED = [
    ('avx-512\\+champagne_lemire', 'Champagne--Lemire'),
    ('jeaiii_fast_uint64', 'jeaiii'),
    ('itoa_yy_64', 'yy'),
    ('std::to_chars', 'std::to_chars'),
    ('mula_sse64', 'Mula SSE64'),
]

# Digit lengths to include (1-20 digits for uint64_t)
DIGIT_LENGTHS = list(range(1, 21))

# Color palette for algorithms
COLORS = [
    '#e41a1c', '#377eb8', '#4daf4a', '#984ea3', '#ff7f00',
    '#a65628', '#f781bf', '#999999', '#66c2a5', '#8da0cb'
]


def parse_algorithm_metrics(file_path: Path, algorithm_pattern: str) -> Optional[Dict[str, float]]:
    """Parse benchmark output and extract all metrics for a specific algorithm."""
    try:
        with open(file_path, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        return None

    escaped_pattern = algorithm_pattern
    pattern = rf'{escaped_pattern}\s*:\s*([\d.]+)\s*ns/n\s*[\d.]+\s*GHz\s*([\d.]+)\s*c/n\s*([\d.]+)\s*i/n'

    match = re.search(pattern, content)
    if not match:
        return None
    return {
        'ns/n': float(match.group(1)),
        'c/n': float(match.group(2)),
        'i/n': float(match.group(3)),
    }


def find_dataset_files(base_name: str, output_dir: Path, compiler: str, cpu_model: Optional[str]) -> Tuple[Optional[Path], Optional[Path]]:
    """Find both -homo and -hetero files for a dataset."""
    patterns = []
    if cpu_model:
        patterns.append(f"{cpu_model}_{compiler}_{base_name}-{{variant}}.raw")
    patterns.append(f"{base_name}-{{variant}}.raw")

    for pattern_template in patterns:
        homo_file = output_dir / pattern_template.format(variant="homo")
        hetero_file = output_dir / pattern_template.format(variant="hetero")
        if homo_file.exists() or hetero_file.exists():
            return (homo_file if homo_file.exists() else None,
                    hetero_file if hetero_file.exists() else None)

    return None, None


def extract_auto_selection(file_path: Path) -> Optional[str]:
    """Extract which variant was auto-selected from benchmark output."""
    try:
        with open(file_path, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        return None

    match = re.search(r'Auto-detected best variant: (\w+)', content)
    if match:
        return match.group(1).lower()
    return None


def auto_detect_cpu_model(output_dir: Path, compiler: str) -> Optional[str]:
    """Auto-detect CPU model from existing .raw files."""
    # Look for any .raw file with the compiler name
    pattern = f"*_{compiler}_*.raw"
    files = list(output_dir.glob(pattern))

    if not files:
        return None

    # Extract CPU model from first matching file
    # Pattern: {cpu_model}_{compiler}_{dataset}-{variant}.raw
    filename = files[0].stem  # Remove .raw extension
    parts = filename.split(f"_{compiler}_")
    if len(parts) == 2:
        return parts[0]

    return None


def get_available_compilers(output_dir: Path) -> List[str]:
    """Detect which compilers have data available."""
    compilers = []
    for compiler in ["g++", "clang++"]:
        pattern = f"*_{compiler}_*.raw"
        if list(output_dir.glob(pattern)):
            compilers.append(compiler)
    return compilers


def collect_data_by_digit_length(output_dir: Path, compiler: str, cpu_model: Optional[str], algorithms: List[Tuple[str, str]]) -> Dict[int, Dict[str, float]]:
    """
    Collect performance data for all digit lengths.
    Returns: {digit_length: {algorithm_name: ns/n_value}}
    """
    data = {}
    for digit_length in DIGIT_LENGTHS:
        dataset_base = f"uniform-{digit_length}digit-1M"
        homo_file, hetero_file = find_dataset_files(dataset_base, output_dir, compiler, cpu_model)

        if not homo_file and not hetero_file:
            print(f"  WARNING: No data files found for {digit_length}-digit dataset")
            continue

        # Determine which variant to use (prefer auto-detected, fallback to homo)
        selected_file = None
        if homo_file:
            variant = extract_auto_selection(homo_file)
            if variant == "homo":
                selected_file = homo_file
            elif variant == "hetero" and hetero_file:
                selected_file = hetero_file
            else:
                selected_file = homo_file
        elif hetero_file:
            selected_file = hetero_file

        if not selected_file:
            continue

        # Extract metrics for all algorithms
        digit_data = {}
        for algo_pattern, algo_display in algorithms:
            metrics = parse_algorithm_metrics(selected_file, algo_pattern)
            if metrics and 'ns/n' in metrics:
                digit_data[algo_display] = metrics['ns/n']

        if digit_data:
            data[digit_length] = digit_data

    return data


def generate_figure(data: Dict[int, Dict[str, float]], output_path: Path, metric_name: str = "ns/n"):
    """Generate a line plot comparing algorithms across digit lengths."""
    if not data:
        print("ERROR: No data to plot")
        return

    # Extract algorithm names from first available dataset
    algo_names = list(next(iter(data.values())).keys())

    # Prepare data for plotting
    digit_lengths = sorted(data.keys())
    lines_data = {algo: [] for algo in algo_names}

    for length in digit_lengths:
        length_data = data[length]
        for algo in algo_names:
            value = length_data.get(algo)
            lines_data[algo].append(value if value is not None else float('nan'))

    # Configure matplotlib for LaTeX output
    # Font sizes compensate for 2.1× scale-down when included at full linewidth
    # Figure: 11.9" wide → Paper: 5.6" wide → 10pt fonts would appear as 4.8pt
    # Using 20pt base ensures ~10pt appearance in final paper
    plt.rcParams.update({
        'font.size': 20,           # Base font size (scales down to ~10pt)
        'axes.labelsize': 20,      # Axis labels
        'axes.titlesize': 22,      # Title (slightly larger)
        'xtick.labelsize': 18,     # X-axis tick labels
        'ytick.labelsize': 18,     # Y-axis tick labels
        'legend.fontsize': 18,     # Legend
        'font.family': 'serif',    # Use serif fonts (matches LaTeX)
        'text.usetex': True,       # Use LaTeX for text rendering
    })

    # Create figure
    plt.figure(figsize=(12, 7))

    # Plot each algorithm
    for idx, algo in enumerate(algo_names):
        color = COLORS[idx % len(COLORS)]
        plt.plot(digit_lengths, lines_data[algo], marker='o', linewidth=2,
                 markersize=6, label=algo, color=color)

    plt.xlabel('Number of Digits')
    plt.ylabel(f'Performance ({metric_name})')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.legend(loc='best', framealpha=0.9, ncol=2)
    plt.xticks(digit_lengths)
    plt.tight_layout()

    # Save figure
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"\nFigure saved: {output_path}")
    plt.close()


def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Generate digit length comparison figure from benchmark outputs")
    parser.add_argument(
        "--compiler",
        default=None,
        help="Compiler to use for finding output files. If not specified, generates figures for all available compilers.")
    parser.add_argument(
        "--cpu-model",
        default=None,
        help="CPU model prefix in filenames (auto-detected if not specified)")
    parser.add_argument(
        "--input-dir",
        default="./outputs",
        help="Directory containing raw benchmark files (default: ./outputs)")
    parser.add_argument(
        "--output-dir",
        default="./outputs",
        help="Directory to save generated figure (default: ./outputs)")
    parser.add_argument(
        "--simplified",
        action="store_true",
        help="Generate simplified figure with 5 key algorithms for main paper")
    return parser.parse_args()


def process_compiler(compiler: str, cpu_model: Optional[str], input_dir: Path, output_dir: Path, simplified: bool = False):
    """Process data and generate figure for a single compiler."""
    suffix = "_simplified" if simplified else ""
    figure_path = output_dir / f"figure_digit_length_comparison_{compiler}{suffix}.pdf"

    # Auto-detect CPU model if not provided
    detected_cpu = cpu_model
    if detected_cpu is None:
        detected_cpu = auto_detect_cpu_model(input_dir, compiler)
        if detected_cpu:
            print(f"Auto-detected CPU model: {detected_cpu}")

    print(f"Reading from: {input_dir}")
    print(f"Compiler: {compiler}")
    if detected_cpu:
        print(f"CPU model: {detected_cpu}")
    else:
        print("No CPU model (looking for files without CPU prefix)")
    print()

    # Collect data
    algorithms = ALGORITHMS_SIMPLIFIED if simplified else ALGORITHMS
    data = collect_data_by_digit_length(input_dir, compiler, detected_cpu, algorithms)

    if not data:
        print(f"ERROR: No data collected for {compiler}. Make sure benchmark outputs exist for uniform-Ndigit-1M datasets.")
        return False

    algo_count = len(algorithms)
    print(f"Collected data for {len(data)} digit lengths ({algo_count} algorithms)")

    # Generate figure
    generate_figure(data, figure_path)
    return True


def main():
    args = parse_args()

    input_dir = Path(args.input_dir)
    output_dir = Path(args.output_dir)

    if not input_dir.exists():
        print(f"ERROR: Input directory not found: {input_dir}")
        sys.exit(1)

    # Determine which compilers to process
    if args.compiler:
        compilers = [args.compiler]
    else:
        compilers = get_available_compilers(input_dir)
        if not compilers:
            print("ERROR: No compiler data found in output directory.")
            print("Run benchmarks first or specify --compiler explicitly.")
            sys.exit(1)
        print(f"Auto-detected compilers with data: {', '.join(compilers)}\n")

    print("Generating digit-length comparison figures...")
    if args.simplified:
        print("Mode: SIMPLIFIED (5 algorithms for main paper)")
    else:
        print("Mode: FULL (all 10 algorithms)")

    success_count = 0
    for compiler in compilers:
        print(f"\n{'='*60}")
        print(f"Processing compiler: {compiler}")
        print('='*60)
        if process_compiler(compiler, args.cpu_model, input_dir, output_dir, args.simplified):
            success_count += 1

    if success_count == 0:
        sys.exit(1)

    print(f"\nGenerated {success_count} figure(s) successfully.")


if __name__ == "__main__":
    main()
