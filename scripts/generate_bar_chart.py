#!/usr/bin/env python3
"""
Generate bar chart figures comparing integer-to-string algorithms across datasets.

Creates publication-quality bar charts showing performance (ns/digit) for all algorithms
across three key datasets: Twitter JSON, CIT Patents, and Natural 1-8 digit.

Based on scripts/daniel/plotfiles.py but reads from benchmark output files.
"""

import sys
import re
import argparse
from pathlib import Path
from typing import Dict, List, Optional, Tuple
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend

from utils import get_cpu_model

# Algorithm list (same as table scripts)
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

# Datasets to include in bar charts
DATASETS = [
    ("twitterjson_integers", "Twitter JSON"),
    ("cit_patents_citing_integers", "CIT Patents"),
    ("natural-8-1M", "Natural 1-8 digit"),
]

# Color palette for algorithms (colorblind-friendly)
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


def extract_auto_selection(file_path: Path) -> Optional[str]:
    """Extract the auto-selected variant from the output file."""
    try:
        with open(file_path, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        return None

    pattern = r'Auto variant would select:\s*(Homogeneous|Heterogeneous)'
    match = re.search(pattern, content)
    return match.group(1) if match else None


def find_dataset_files(base_name: str, compiler: str, output_dir: Path) -> Tuple[Optional[Path], Optional[Path]]:
    """Find output files for a dataset. Returns (homo_file, hetero_file)."""
    cpu_model = get_cpu_model().replace(' ', '_').replace('/', '-').replace('@', '')

    # Try with CPU model and compiler prefix
    homo_file = output_dir / f"{cpu_model}_{compiler}_{base_name}-homo.raw"
    hetero_file = output_dir / f"{cpu_model}_{compiler}_{base_name}-hetero.raw"

    # Fallback: try without CPU model prefix
    if not homo_file.exists():
        homo_file = output_dir / f"{base_name}-homo.raw"
        hetero_file = output_dir / f"{base_name}-hetero.raw"

    return (homo_file if homo_file.exists() else None,
            hetero_file if hetero_file.exists() else None)


def get_algorithm_results_for_dataset(dataset_name: str, compiler: str,
                                      output_dir: Path, algorithms: List[Tuple[str, str]]) -> Optional[Dict[str, Dict[str, float]]]:
    """Get all algorithm results for a specific dataset."""
    homo_file, hetero_file = find_dataset_files(dataset_name, compiler, output_dir)
    if homo_file is None and hetero_file is None:
        return None

    # Determine which variant was auto-selected
    selected = None
    if homo_file:
        selected = extract_auto_selection(homo_file)
    if selected is None and hetero_file:
        selected = extract_auto_selection(hetero_file)

    # Use the auto-selected variant's file
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
    for algo_pattern, algo_display_name in algorithms:
        metrics = parse_algorithm_metrics(file_path, algo_pattern)
        if metrics is not None:
            results[algo_display_name] = metrics
    return results if results else None


def get_available_compilers(output_dir: Path) -> List[str]:
    """Detect which compilers have data available."""
    compilers = []
    for compiler in ["g++", "clang++"]:
        pattern = f"*_{compiler}_*.raw"
        if list(output_dir.glob(pattern)):
            compilers.append(compiler)
    return compilers


def collect_data(output_dir: Path, compiler: str, algorithms: List[Tuple[str, str]]) -> Dict[str, Dict[str, float]]:
    """
    Collect ns/d data for all datasets and algorithms.
    Returns: {dataset_display: {algorithm_name: ns/d_value}}
    """
    data = {}
    for dataset_base, dataset_display in DATASETS:
        results = get_algorithm_results_for_dataset(dataset_base, compiler, output_dir, algorithms)
        if results:
            data[dataset_display] = {algo: metrics['ns/d'] for algo, metrics in results.items()}
        else:
            print(f"  WARNING: No data found for dataset {dataset_display}")
    return data


def generate_bar_chart(data: Dict[str, Dict[str, float]], algorithms: List[Tuple[str, str]],
                       output_path: Path, combined: bool = True):
    """Generate bar chart figure comparing algorithms across datasets."""
    if not data:
        print("ERROR: No data to plot")
        return

    algo_names = [display for _, display in algorithms]

    # Configure matplotlib for LaTeX output
    # Font sizes compensate for scale-down when included at full linewidth
    # Match generate_digit_length_figure.py settings for consistency
    plt.rcParams.update({
        'font.size': 20,           # Base font size (scales down to ~10pt)
        'axes.labelsize': 20,      # Axis labels
        'axes.titlesize': 22,      # Title (slightly larger)
        'xtick.labelsize': 14,     # X-axis tick labels (smaller for rotated algo names)
        'ytick.labelsize': 18,     # Y-axis tick labels
        'legend.fontsize': 16,     # Legend
        'font.family': 'serif',
        'text.usetex': True,
    })

    if combined:
        # Combined figure with 3 subplots (one per dataset)
        fig, axes = plt.subplots(3, 1, figsize=(12, 16))

        for idx, (dataset_display, dataset_data) in enumerate(data.items()):
            ax = axes[idx]
            values = [dataset_data.get(algo, 0) for algo in algo_names]
            colors = COLORS[:len(algo_names)]

            bars = ax.bar(algo_names, values, color=colors, edgecolor='black', linewidth=0.8)

            # Add value labels on bars
            for bar, val in zip(bars, values):
                if val > 0:
                    ax.text(bar.get_x() + bar.get_width()/2, val + 0.03,
                            f"{val:.2f}", ha='center', va='bottom', fontsize=14)

            ax.set_ylabel("ns per digit")
            ax.set_title(dataset_display)
            ax.set_xticks(range(len(algo_names)))
            ax.set_xticklabels(algo_names, rotation=45, ha='right')
            ax.grid(True, axis='y', linestyle='--', alpha=0.4)
            ax.spines['top'].set_visible(False)
            ax.spines['right'].set_visible(False)
            ax.set_ylim(0, max(values) * 1.18 if values and max(values) > 0 else 1)

        plt.tight_layout()
        fig.savefig(output_path, format="pdf", bbox_inches="tight", dpi=200)
        plt.close(fig)
    else:
        # Separate figures for each dataset
        for dataset_display, dataset_data in data.items():
            values = [dataset_data.get(algo, 0) for algo in algo_names]
            colors = COLORS[:len(algo_names)]

            fig, ax = plt.subplots(figsize=(12, 7))
            bars = ax.bar(algo_names, values, color=colors, edgecolor='black', linewidth=0.8)

            for bar, val in zip(bars, values):
                if val > 0:
                    ax.text(bar.get_x() + bar.get_width()/2, val + 0.03,
                            f"{val:.2f}", ha='center', va='bottom', fontsize=14)

            ax.set_ylabel("ns per digit")
            ax.set_title(dataset_display)
            ax.set_xticks(range(len(algo_names)))
            ax.set_xticklabels(algo_names, rotation=45, ha='right')
            ax.grid(True, axis='y', linestyle='--', alpha=0.4)
            ax.spines['top'].set_visible(False)
            ax.spines['right'].set_visible(False)
            ax.set_ylim(0, max(values) * 1.18 if values and max(values) > 0 else 1)

            plt.tight_layout()

            # Create filename from dataset name
            dataset_slug = dataset_display.lower().replace(' ', '_').replace('-', '_')
            separate_path = output_path.parent / f"figure_bar_chart_{dataset_slug}_{output_path.stem.split('_')[-1]}.pdf"
            fig.savefig(separate_path, format="pdf", bbox_inches="tight", dpi=200)
            print(f"  Saved: {separate_path}")
            plt.close(fig)


def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Generate bar chart figures comparing algorithms across datasets")
    parser.add_argument(
        "--compiler",
        default=None,
        help="Compiler to use for finding output files. If not specified, generates figures for all available compilers.")
    parser.add_argument(
        "--input-dir",
        default="./outputs",
        help="Directory containing raw benchmark files (default: ./outputs)")
    parser.add_argument(
        "--output-dir",
        default="./outputs",
        help="Directory to save generated figures (default: ./outputs)")
    parser.add_argument(
        "--simplified",
        action="store_true",
        help="Generate simplified figure with 5 key algorithms for main paper")
    parser.add_argument(
        "--separate",
        action="store_true",
        help="Generate separate figures for each dataset instead of combined")
    return parser.parse_args()


def process_compiler(compiler: str, input_dir: Path, output_dir: Path, simplified: bool, separate: bool) -> bool:
    """Process data and generate bar chart for a single compiler."""
    algorithms = ALGORITHMS_SIMPLIFIED if simplified else ALGORITHMS
    suffix = "_simplified" if simplified else ""
    figure_path = output_dir / f"figure_bar_chart_{compiler}{suffix}.pdf"

    print(f"Reading from: {input_dir}")
    print(f"Compiler: {compiler}")
    print(f"Algorithms: {len(algorithms)}")
    print()

    # Collect data
    data = collect_data(input_dir, compiler, algorithms)

    if not data:
        print(f"ERROR: No data collected for {compiler}.")
        return False

    print(f"Collected data for {len(data)} datasets")

    # Generate figure
    generate_bar_chart(data, algorithms, figure_path, combined=not separate)
    if not separate:
        print(f"\nFigure saved: {figure_path}")
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

    print("Generating bar chart figures...")
    if args.simplified:
        print("Mode: SIMPLIFIED (5 algorithms for main paper)")
    else:
        print("Mode: FULL (all 10 algorithms)")

    success_count = 0
    for compiler in compilers:
        print(f"\n{'='*60}")
        print(f"Processing compiler: {compiler}")
        print('='*60)
        if process_compiler(compiler, input_dir, output_dir, args.simplified, args.separate):
            success_count += 1

    if success_count == 0:
        sys.exit(1)

    print(f"\nGenerated {success_count} figure(s) successfully.")


if __name__ == "__main__":
    main()
