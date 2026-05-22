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

- Metrics CSV: <a href="results/notebook01_input_distribution_metrics.csv">`results/notebook01_input_distribution_metrics.csv`</a>
- Metrics JSON: <a href="results/notebook01_input_distribution_metrics.json">`results/notebook01_input_distribution_metrics.json`</a>
- Figure: <a href="figures/notebook01_entropy_vs_repetition.png">`figures/notebook01_entropy_vs_repetition.png`</a>
- Figure: <a href="figures/notebook01_delta_abs_mean.png">`figures/notebook01_delta_abs_mean.png`</a>

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

- Metrics CSV: <a href="results/notebook02_cache_branching_metrics.csv">`results/notebook02_cache_branching_metrics.csv`</a>
- Metrics JSON: <a href="results/notebook02_cache_branching_metrics.json">`results/notebook02_cache_branching_metrics.json`</a>
- Figure: <a href="figures/notebook02_branch_pressure_score.png">`figures/notebook02_branch_pressure_score.png`</a>
- Figure: <a href="figures/notebook02_digit_entropy_vs_transition.png">`figures/notebook02_digit_entropy_vs_transition.png`</a>
- Figure: <a href="figures/notebook02_locality_vs_reuse.png">`figures/notebook02_locality_vs_reuse.png`</a>
- Figure: <a href="figures/notebook02_delta_abs_mean_log.png">`figures/notebook02_delta_abs_mean_log.png`</a>

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

- Metrics CSV: <a href="results/notebook03_simd_scalar_path_metrics.csv">`results/notebook03_simd_scalar_path_metrics.csv`</a>
- Metrics JSON: <a href="results/notebook03_simd_scalar_path_metrics.json">`results/notebook03_simd_scalar_path_metrics.json`</a>
- Figure: <a href="figures/notebook03_simd_suitability.png">`figures/notebook03_simd_suitability.png`</a>
- Figure: <a href="figures/notebook03_scalar_vs_simd_throughput.png">`figures/notebook03_scalar_vs_simd_throughput.png`</a>
- Figure: <a href="figures/notebook03_speedup_vs_branch_pressure.png">`figures/notebook03_speedup_vs_branch_pressure.png`</a>
- Figure: <a href="figures/notebook03_execution_path_phase_map.png">`figures/notebook03_execution_path_phase_map.png`</a>

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

- Metrics CSV: <a href="results/notebook04_constraint_phase_metrics.csv">`results/notebook04_constraint_phase_metrics.csv`</a>
- Metrics JSON: <a href="results/notebook04_constraint_phase_metrics.json">`results/notebook04_constraint_phase_metrics.json`</a>
- Figure: <a href="figures/notebook04_constraint_phase_map.png">`figures/notebook04_constraint_phase_map.png`</a>
- Figure: <a href="figures/notebook04_coherence_landscape.png">`figures/notebook04_coherence_landscape.png`</a>
- Figure: <a href="figures/notebook04_regime_classification.png">`figures/notebook04_regime_classification.png`</a>
- Figure: <a href="figures/notebook04_constraint_summary_matrix.png">`figures/notebook04_constraint_summary_matrix.png`</a>

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

# Report 05 — Real Benchmark Ingestion

This report ingests benchmark outputs and compares observed performance against RML structural phase metrics.

Constraint view:
> useful models become stronger when they meet measured systems behavior.

## Generated outputs

- Metrics CSV: <a href="results/notebook05_real_benchmark_ingestion.csv">`results/notebook05_real_benchmark_ingestion.csv`</a>
- Metrics JSON: <a href="results/notebook05_real_benchmark_ingestion.json">`results/notebook05_real_benchmark_ingestion.json`</a>
- Figure: <a href="figures/notebook05_observed_throughput.png">`figures/notebook05_observed_throughput.png`</a>
- Figure: <a href="figures/notebook05_observed_latency.pn">`figures/notebook05_observed_latency.png`</a>
- Figure: <a href="figures/notebook05_coherence_vs_throughput.png">`figures/notebook05_coherence_vs_throughput.png`</a>
- Figure: <a href="igures/notebook05_prediction_gap.png">`figures/notebook05_prediction_gap.png`</a>

## Benchmark summary

| distribution          |   observed_throughput_mib_s |   observed_latency_ns |   runs | regime               |   coherence_score |   fragmentation_score |   observed_throughput_norm |   abs_prediction_gap | implementations   | simd_modes   | hardware_profiles   |
|:----------------------|----------------------------:|----------------------:|-------:|:---------------------|------------------:|----------------------:|---------------------------:|---------------------:|:------------------|:-------------|:--------------------|
| clustered_ranges      |                         950 |                  1.05 |      1 | fragmented-irregular |              0.22 |                  1    |                   0        |            0.22      | mixed_path        | mixed        | baseline            |
| low_entropy_repeating |                        1650 |                  0.6  |      1 | coherent-local       |              0.75 |                  0.02 |                   0.736842 |            0.0131579 | scalar_fastpath   | scalar       | baseline            |
| sequential_ids        |                        1350 |                  0.74 |      1 | scalar-favorable     |              0.52 |                  0.36 |                   0.421053 |            0.0989474 | scalar_fastpath   | scalar       | baseline            |
| uniform_32bit         |                        1900 |                  0.53 |      1 | simd-favorable       |              0.24 |                  0.95 |                   1        |            0.76      | simd_path         | SIMD         | baseline            |
| zipfian_smallints     |                        1500 |                  0.67 |      1 | simd-favorable       |              0.42 |                  0.79 |                   0.578947 |            0.158947  | simd_path         | SIMD         | baseline            |

## Interpretation

- Observed throughput and latency are now connected to structural predictions.
- Prediction gaps identify where the proxy model needs correction or real hardware context.
- High coherence does not automatically mean maximum throughput; it means structural alignment under chosen constraints.
- This notebook creates the bridge from RML proxy analysis to real benchmark validation.

## Next step

Notebook 06 should add hardware-counter overlays or architecture-specific benchmark comparisons.

## Notebook 06 — Hardware Counter Overlays

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/06_hardware_counter_overlays.ipynb
)

# Report 06 — Hardware-Counter Overlays

This report overlays hardware-counter metrics onto RML phase and benchmark results.

Constraint view:
> performance becomes interpretable when structural predictions meet hardware counters.

## Generated outputs

- Metrics CSV: <a href="results/notebook06_hardware_counter_overlays.csv">`results/notebook06_hardware_counter_overlays.csv`</a>
- Metrics JSON: <a href="results/notebook06_hardware_counter_overlays.json`">`results/notebook06_hardware_counter_overlays.json`</a>
- Figure: <a href="figures/notebook06_branch_miss_rate.png">`figures/notebook06_branch_miss_rate.png`</a>
- Figure: <a href="figures/notebook06_cache_miss_rate.png">`figures/notebook06_cache_miss_rate.png`</a>
- Figure: <a href="figures/notebook06_fragmentation_vs_counter_pressure.png">`figures/notebook06_fragmentation_vs_counter_pressure.png`</a>
- Figure: <a href="figures/notebook06_ipc_vs_throughput.png">`figures/notebook06_ipc_vs_throughput.png`</a>
- Figure: <a href="figures/notebook06_counter_summary_matrix.png">`figures/notebook06_counter_summary_matrix.png`</a>

## Hardware-counter overlay summary

| distribution          | regime               |   observed_throughput_mib_s |   observed_latency_ns |   branch_miss_rate |   cache_miss_rate |     ipc |   cycles_per_item |   fragmentation_score |   counter_pressure_score |   abs_fragmentation_counter_gap |
|:----------------------|:---------------------|----------------------------:|----------------------:|-------------------:|------------------:|--------:|------------------:|----------------------:|-------------------------:|--------------------------------:|
| low_entropy_repeating | coherent-local       |                        1650 |                  0.6  |          0.0005    |         0.005     | 2       |               900 |                  0.02 |                0.0453782 |                     0.0253782   |
| sequential_ids        | scalar-favorable     |                        1350 |                  0.74 |          0.0048    |         0.0122222 | 1.75    |              1200 |                  0.36 |                0.238392  |                     0.121608    |
| uniform_32bit         | simd-favorable       |                        1900 |                  0.53 |          0.0285714 |         0.022     | 2.6     |              1000 |                  0.95 |                0.411201  |                     0.538799    |
| zipfian_smallints     | simd-favorable       |                        1500 |                  0.67 |          0.0306452 |         0.01875   | 1.84615 |              1300 |                  0.79 |                0.516925  |                     0.273075    |
| clustered_ranges      | fragmented-irregular |                         950 |                  1.05 |          0.0466667 |         0.0387755 | 1.27778 |              1800 |                  1    |                1         |                     1.11022e-16 |

## Interpretation

- Branch miss rate and cache miss rate help convert structural proxies into measurable systems behavior.
- Counter pressure can validate, correct, or refine the fragmentation model from Notebook 04.
- IPC vs throughput helps distinguish hardware saturation from structural coherence.
- Real counters are the next bridge from RML phase maps to architecture-aware performance claims.

## Next step

Notebook 07 can build adaptive path-selection rules using structural metrics plus observed counters.

## Notebook 07 — Adaptive Execution Selection

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/07_adaptive_execution_selection.ipynb
)

# Report 07 — Adaptive Execution Selection

This report turns RML structural, benchmark, and hardware-counter layers into an adaptive execution-selector model.

Constraint view:
> execution is a constraint-selection problem: distribution structure → execution topology → hardware pressure → adaptive path choice.

## Generated outputs

- Metrics CSV: <a href="results/notebook07_adaptive_execution_selection.csv">`results/notebook07_adaptive_execution_selection.csv`</a>
- Metrics JSON: <a href="results/notebook07_adaptive_execution_selection.json">`results/notebook07_adaptive_execution_selection.json`</a>
- Figure: <a href="figures/notebook07_selector_scores.png">`figures/notebook07_selector_scores.png`</a>
- Figure: <a href="figures/notebook07_selector_phase_map.png">`figures/notebook07_selector_phase_map.png`</a>
- Figure: <a href="figures/notebook07_predicted_vs_observed.png">`figures/notebook07_predicted_vs_observed.png`</a>
- Figure: <a href="figures/notebook07_adaptive_improvement_opportunity.png">`figures/notebook07_adaptive_improvement_opportunity.png`</a>
- Figure: <a href="figures/notebook07_policy_matrix.png">`figures/notebook07_policy_matrix.png`</a>

## Adaptive selector summary

| distribution          | predicted_selector      | observed_behavior_label   | selector_matches_observed   |   selector_confidence |   coherence_score |   hardware_pressure |   observed_throughput_mib_s |   adaptive_improvement_opportunity |   estimated_adaptive_gain_pct |
|:----------------------|:------------------------|:--------------------------|:----------------------------|----------------------:|------------------:|--------------------:|----------------------------:|-----------------------------------:|------------------------------:|
| low_entropy_repeating | select_scalar           | select_coherent_local     | False                       |             0.0345385 |              0.75 |           0.0821818 |                        1650 |                           0.122255 |                       8.05637 |
| sequential_ids        | select_hybrid           | select_scalar             | False                       |             0.0166181 |              0.52 |           0.25518   |                        1350 |                           0.32     |                      13       |
| uniform_32bit         | select_simd             | select_simd               | True                        |             0.133568  |              0.24 |           0.416349  |                        1900 |                           0.519857 |                      17.9964  |
| zipfian_smallints     | select_hybrid           | select_hybrid             | True                        |             0.0993733 |              0.42 |           0.530647  |                        1500 |                           0.515291 |                      17.8823  |
| clustered_ranges      | select_guarded_fallback | select_guarded_fallback   | True                        |             0.563125  |              0.22 |           1         |                         950 |                           0.8      |                      25       |

## Interpretation

- The selector treats scalar, SIMD, coherent-local, guarded-fallback, and hybrid execution as regimes.
- Selector confidence measures how clearly one regime dominates the alternatives.
- Mismatches identify where the rule-based policy needs measured benchmark correction.
- Improvement opportunity highlights distributions where adaptive routing could matter most.

## Next step

Notebook 08 can simulate online runtime adaptation: classify distribution windows, choose execution paths, and estimate throughput under switching costs.

## Notebook 08 — Streaming Runtime Adaptation

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/08_streaming_runtime_adaptation.ipynb
)

# Report 08 — Streaming Runtime Adaptation

This report simulates adaptive execution selection over a regime-switching integer stream.

Constraint view:
> runtime adaptation asks whether coherence persists as distributions drift.

## Generated outputs

- Metrics CSV: <a href="results/notebook08_streaming_runtime_adaptation.csv">`results/notebook08_streaming_runtime_adaptation.csv`</a>
- Metrics JSON: <a href="results/notebook08_streaming_runtime_adaptation.json">`results/notebook08_streaming_runtime_adaptation.json`</a>
- Figure: <a href="figures/notebook08_regime_timeline.png">`figures/notebook08_regime_timeline.png`</a>
- Figure: <a href="figures/notebook08_policy_timeline.png">`figures/notebook08_policy_timeline.png`</a>
- Figure: <a href="figures/notebook08_throughput_timeline.png">`figures/notebook08_throughput_timeline.png`</a>
- Figure: <a href="figures/notebook08_coherence_pressure_timeline.png">`figures/notebook08_coherence_pressure_timeline.png`</a>
- Figure: <a href="figures/notebook08_adaptive_gain_by_regime.png">`figures/notebook08_adaptive_gain_by_regime.png`</a>

## Summary

|   windows |   policy_switches |   mean_fixed_scalar |   mean_fixed_simd |   mean_adaptive |   mean_gain_pct_vs_best_fixed |
|----------:|------------------:|--------------------:|------------------:|----------------:|------------------------------:|
|        32 |                 8 |              1287.5 |           1493.75 |         1420.42 |                      -5.54265 |

## Policy counts

| policy           |   count |
|:-----------------|--------:|
| guarded_fallback |      13 |
| coherent_local   |       8 |
| simd             |       8 |
| hybrid           |       3 |

## Regime × policy table

| truth_regime          |   coherent_local |   guarded_fallback |   hybrid |   simd |
|:----------------------|-----------------:|-------------------:|---------:|-------:|
| clustered_ranges      |                0 |                  4 |        0 |      0 |
| low_entropy_repeating |                8 |                  0 |        0 |      0 |
| sequential_ids        |                0 |                  0 |        0 |      8 |
| uniform_32bit         |                0 |                  8 |        0 |      0 |
| zipfian_smallints     |                0 |                  1 |        3 |      0 |

## Interpretation

- The adaptive selector tracks changes in local distribution structure across windows.
- Switching costs prevent unrealistic free adaptation.
- Coherence and hardware-pressure timelines expose when the stream becomes stable or fragmented.
- Adaptive gain is most meaningful in mixed or high-pressure regimes where fixed scalar/SIMD policies are brittle.

## Next step

Notebook 09 can test cross-hardware policy portability: does the same selector transfer across x86, ARM, AVX2, AVX512, and cloud baselines?
