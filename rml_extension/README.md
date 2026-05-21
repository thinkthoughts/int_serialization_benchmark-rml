# RML Extension

## Notebook 01 — Input Distribution Analysis

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/01_input_distribution_analysis.ipynb
)

# Report 01 — Input Distribution Analysis

This report initializes the RML extension for `int_serialization_benchmark-rml`.

Constraint view:
> serialization performance depends on data distribution, entropy, repetition, locality, and hardware pathways.

## Generated outputs

- Metrics CSV: `/content/results/notebook01_input_distribution_metrics.csv`
- Metrics JSON: `/content/results/notebook01_input_distribution_metrics.json`
- Figure: `/content/figures/notebook01_entropy_vs_repetition.png`
- Figure: `/content/figures/notebook01_delta_abs_mean.png`

## Distribution metrics

| name                  |      n |   min |                 max |   unique_count |   repetition_ratio |   approx_entropy_bits |      delta_mean |       delta_std |   delta_abs_mean |
|:----------------------|-------:|------:|--------------------:|---------------:|-------------------:|----------------------:|----------------:|----------------:|-----------------:|
| zipfian_smallints     | 100000 |     1 | 6710887487477844992 |          19448 |            0.80552 |             0.0105474 |    -0.0180902   |     6.42075e+16 |      1.05351e+15 |
| low_entropy_repeating | 100000 |     1 |                   4 |              4 |            0.99996 |             2         |     3.00003e-05 |     1.73203     |      1.49998     |
| clustered_ranges      | 100000 |     0 |              100999 |           2000 |            0.98    |             3.39154   |     1.00196     | 70805.6         |  50299.3         |
| uniform_32bit         | 100000 |  3736 |          4294940379 |          99999 |            1e-05   |             8.99628   | 36419.9         |     1.75598e+09 |      1.43465e+09 |
| sequential_ids        | 100000 |     0 |               99999 |         100000 |            0       |             9         |     1           |     0           |      1           |

## Interpretation

- Low-entropy or repeating distributions may favor branch/cache-friendly pathways.
- High-entropy distributions stress parsing and serialization throughput differently.
- Sequential and clustered inputs expose locality and delta structure.
- Later notebooks can overlay throughput, latency, SIMD mode, and hardware profile results.

## Notebook 02 — Cache & Branching Structure

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/02_cache_branching_structure.ipynb
)

# Report 02 — Cache & Branching Structure

This report adds a structural proxy layer for cache and branching behavior in integer serialization.

Constraint view:
> serialization performance emerges from interactions among digit structure, branch transitions, locality, reuse, and hardware pathways.

## Generated outputs

- Metrics CSV: `/content/results/notebook02_cache_branching_metrics.csv`
- Metrics JSON: `/content/results/notebook02_cache_branching_metrics.json`
- Figure: `/content/figures/notebook02_branch_pressure_score.png`
- Figure: `/content/figures/notebook02_digit_entropy_vs_transition.png`
- Figure: `/content/figures/notebook02_locality_vs_reuse.png`
- Figure: `/content/figures/notebook02_delta_abs_mean_log.png`

## Cache / branching proxy metrics

| name                  |      n |   digit_length_min |   digit_length_max |   digit_length_entropy |   digit_length_transition_rate |   locality_small_delta_ratio |   cache_window_reuse_proxy |   unique_ratio |   repetition_ratio |   branch_pressure_score |   delta_abs_mean |
|:----------------------|-------:|-------------------:|-------------------:|-----------------------:|-------------------------------:|-----------------------------:|---------------------------:|---------------:|-------------------:|------------------------:|-----------------:|
| clustered_ranges      | 100000 |                  1 |                  6 |               1.25251  |                    0.545525    |                    0.0160802 |                  0.0152949 |        0.02    |            0.98    |                0.786799 |  50299.3         |
| zipfian_smallints     | 100000 |                  1 |                 10 |               2.41149  |                    0.740837    |                    0.270743  |                  0.349012  |        0.18212 |            0.81788 |                0.718815 |      6.52328e+07 |
| uniform_32bit         | 100000 |                  4 |                 10 |               0.903305 |                    0.365504    |                    0         |                  0         |        0.99999 |            1e-05   |                0.714477 |      1.43465e+09 |
| sequential_ids        | 100000 |                  1 |                  5 |               0.521054 |                    4.00004e-05 |                    1         |                  0         |        1       |            0       |                0.200018 |      1           |
| low_entropy_repeating | 100000 |                  1 |                  1 |              -0        |                    0           |                    1         |                  0.9375    |        4e-05   |            0.99996 |                0.0125   |      1.49998     |

## Interpretation

- Digit-length transitions approximate one source of parsing/formatting branch pressure.
- Small adjacent deltas indicate local continuity that may favor predictable paths.
- Repeated values within local windows act as a simple cache/reuse proxy.
- Heavy-tail distributions require log-scale visualization to avoid masking other regimes.
- Later notebooks can overlay real throughput, latency, SIMD paths, cache misses, and branch-mispredict counters.

## Notebook 03 — SIMD vs Scalar Paths

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/03_simd_vs_scalar_paths.ipynb
)

# Report 03 — SIMD vs Scalar Paths

This report adds an execution-path interpretation layer for integer serialization.

Constraint view:
> SIMD helps when data structure, algorithm path, and hardware constraints align.

## Generated outputs

- Metrics CSV: `/content/results/notebook03_simd_scalar_path_metrics.csv`
- Metrics JSON: `/content/results/notebook03_simd_scalar_path_metrics.json`
- Figure: `/content/figures/notebook03_simd_suitability.png`
- Figure: `/content/figures/notebook03_scalar_vs_simd_throughput.png`
- Figure: `/content/figures/notebook03_speedup_vs_branch_pressure.png`
- Figure: `/content/figures/notebook03_execution_path_phase_map.png`

## Execution-path summary

| name                  |   simd_suitability |   scalar_suitability |   branch_pressure_score |   estimated_scalar_throughput |   estimated_simd_throughput |   estimated_speedup_simd_over_scalar |
|:----------------------|-------------------:|---------------------:|------------------------:|------------------------------:|----------------------------:|-------------------------------------:|
| uniform_32bit         |           0.615411 |            0.0852458 |                    0.72 |                      0.72788  |                    1.05252  |                              1.44601 |
| zipfian_smallints     |           0.668454 |            0.221936  |                    0.72 |                      0.802394 |                    1.1002   |                              1.37114 |
| clustered_ranges      |           0.451659 |            0.0513471 |                    0.79 |                      0.6767   |                    0.815844 |                              1.20562 |
| sequential_ids        |           0.392822 |            0.617615  |                    0.2  |                      1.36671  |                    1.47837  |                              1.0817  |
| low_entropy_repeating |           0.262835 |            0.982     |                    0.01 |                      1.7856   |                    1.4731   |                              0.82499 |

## Interpretation

- SIMD suitability is distribution-dependent, not automatic.
- Branch pressure can reduce SIMD advantage even when wide work exists.
- Scalar pathways remain meaningful for low-entropy, high-reuse, or highly local distributions.
- This notebook creates a modeling layer that later benchmark results can confirm, correct, or falsify.

## Next step

Notebook 04 should combine distribution structure, cache/branching proxies, and execution-path metrics into constraint phase maps.

## Notebook 04 — Constraint Phase Maps

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/04_constraint_phase_maps.ipynb
)

# Report 04 — Constraint Phase Maps

This report combines distribution, cache/branching, and execution-path metrics into an RML-style phase-map layer.

Constraint view:
> performance regimes emerge where distribution structure, branch/cache behavior, and execution paths align.

## Generated outputs

- Metrics CSV: `/content/results/notebook04_constraint_phase_metrics.csv`
- Metrics JSON: `/content/results/notebook04_constraint_phase_metrics.json`
- Figure: `/content/figures/notebook04_constraint_phase_map.png`
- Figure: `/content/figures/notebook04_coherence_landscape.png`
- Figure: `/content/figures/notebook04_regime_classification.png`
- Figure: `/content/figures/notebook04_constraint_summary_matrix.png`

## Constraint phase summary

| name                  | regime               |   coherence_score |   fragmentation_score |   structure_regularization |   execution_alignment |   branch_pressure_score |   simd_suitability |   scalar_suitability |
|:----------------------|:---------------------|------------------:|----------------------:|---------------------------:|----------------------:|------------------------:|-------------------:|---------------------:|
| low_entropy_repeating | coherent-local       |          0.744597 |              0.015    |                  0.981994  |              0.302    |                    0.01 |               0.26 |                 0.98 |
| sequential_ids        | scalar-favorable     |          0.522732 |              0.359615 |                  0.501282  |              0.483079 |                    0.2  |               0.39 |                 0.62 |
| zipfian_smallints     | simd-favorable       |          0.421224 |              0.791115 |                  0.338277  |              0.649206 |                    0.72 |               0.67 |                 0.22 |
| uniform_32bit         | simd-favorable       |          0.242955 |              0.959615 |                  0.0179502 |              0.648    |                    0.72 |               0.62 |                 0.08 |
| clustered_ranges      | fragmented-irregular |          0.224194 |              0.9975   |                  0.15      |              0.44627  |                    0.79 |               0.45 |                 0.05 |

## Interpretation

- Coherence score combines structure regularization, execution alignment, and low fragmentation.
- Fragmentation score highlights branch pressure, weak locality, and weak local reuse.
- SIMD-favorable regimes differ from scalar-favorable regimes; neither is universally best.
- Phase maps create a bridge from synthetic distributions to real benchmark performance.

## Next step

Notebook 05 should integrate real benchmark outputs from upstream runs and compare observed throughput against these structural predictions.

## Notebook 05 — Real Benchmark Ingestion

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/05_real_benchmark_ingestion.ipynb
)

