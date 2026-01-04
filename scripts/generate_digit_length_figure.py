#!/usr/bin/env python3
"""
Generate performance comparison figures for fixed-length digit datasets.
Similar to the itoa-u64-fixed-length.png figure in ibireme/c_numconv_benchmark.
"""

import sys
import re
from pathlib import Path
from typing import Dict, Optional, Tuple
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend

# Algorithm list - same as table script
ALGORITHMS = [
    ('avx-512\\+champagne_lemire', 'AVX-512 (ours)'),
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
    pattern = rf'{escaped_pattern}\s*:\s*([\d.]+)\s*ns/d\s*[\d.]+\s*GHz\s*([\d.]+)\s*c/d\s*([\d.]+)\s*i/d'

    match = re.search(pattern, content)
    if not match:
        return None
    return {
        'ns/d': float(match.group(1)),
        'c/d': float(match.group(2)),
        'i/d': float(match.group(3)),
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


def collect_data_by_digit_length(output_dir: Path, compiler: str, cpu_model: Optional[str]) -> Dict[int, Dict[str, float]]:
    """
    Collect performance data for all digit lengths.
    Returns: {digit_length: {algorithm_name: ns/d_value}}
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
        for algo_pattern, algo_display in ALGORITHMS:
            metrics = parse_algorithm_metrics(selected_file, algo_pattern)
            if metrics and 'ns/d' in metrics:
                digit_data[algo_display] = metrics['ns/d']

        if digit_data:
            data[digit_length] = digit_data

    return data


def generate_figure(data: Dict[int, Dict[str, float]], output_path: Path, metric_name: str = "ns/d"):
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


def main():
    compiler = sys.argv[1] if len(sys.argv) > 1 else "g++"
    cpu_model = sys.argv[2] if len(sys.argv) > 2 else None

    output_dir = Path("./outputs")
    figure_path = output_dir / f"figure_digit_length_comparison_{compiler}.pdf"

    if not output_dir.exists():
        print(f"ERROR: Output directory not found: {output_dir}")
        sys.exit(1)

    # Auto-detect CPU model if not provided
    if cpu_model is None:
        cpu_model = auto_detect_cpu_model(output_dir, compiler)
        if cpu_model:
            print(f"Auto-detected CPU model: {cpu_model}")

    print("Collecting data for digit-length comparison...")
    print(f"Reading from: {output_dir}")
    print(f"Compiler: {compiler}")
    if cpu_model:
        print(f"CPU model: {cpu_model}")
    else:
        print("No CPU model (looking for files without CPU prefix)")
    print()

    # Collect data
    data = collect_data_by_digit_length(output_dir, compiler, cpu_model)

    if not data:
        print("ERROR: No data collected. Make sure benchmark outputs exist for uniform-Ndigit-1M datasets.")
        sys.exit(1)

    print(f"\nCollected data for {len(data)} digit lengths")

    # Generate figure
    generate_figure(data, figure_path)


if __name__ == "__main__":
    main()
