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

## Notebook 09 — Cross-Hardware Policy Portability

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/09_cross_hardware_policy_portability.ipynb
)

# Report 09 — Cross-Hardware Policy Portability

This report tests whether adaptive execution policies remain stable across simulated hardware profiles.

Constraint view:
> a policy is portable only if structure survives architecture changes.

## Generated outputs

- Metrics CSV: <a href="results/notebook09_cross_hardware_policy_portability.csv">`results/notebook09_cross_hardware_policy_portability.csv`</a>
- Metrics JSON: <a href="results/notebook09_cross_hardware_policy_portability.json">`results/notebook09_cross_hardware_policy_portability.json`</a>
- Summary CSV: <a href="results/notebook09_cross_hardware_policy_summary.csv">`results/notebook09_cross_hardware_policy_summary.csv`</a>
- Figure: <a href="figures/notebook09_mean_throughput_by_hardware.png">`figures/notebook09_mean_throughput_by_hardware.png`</a>
- Figure: <a href="figures/notebook09_policy_match_reference.png">`figures/notebook09_policy_match_reference.png`</a>
- Figure: <a href="figures/notebook09_policy_timeline_by_hardware.png">`figures/notebook09_policy_timeline_by_hardware.png`</a>
- Figure: <a href="figures/notebook09_efficiency_by_regime_hardware.png">`figures/notebook09_efficiency_by_regime_hardware.png`</a>
- Figure: <a href="figures/notebook09_portability_summary_matrix.png">`figures/notebook09_portability_summary_matrix.png`</a>

## Hardware portability summary

| hardware_profile   | architecture   |   mean_throughput |   mean_efficiency |   policy_match_reference_rate |   mean_pressure |   policy_switches |   throughput_rank |
|:-------------------|:---------------|------------------:|------------------:|------------------------------:|----------------:|------------------:|------------------:|
| avx2_linux         | x86_64         |           1851.77 |          0.992404 |                         0.375 |         0.51375 |                 5 |                 2 |
| avx512_linux       | x86_64         |           2147.38 |          1        |                         0     |         0.51375 |                 1 |                 1 |
| cloud_vm_baseline  | virtualized    |           1428.64 |          0.990625 |                         0.5   |         0.51375 |                 6 |                 5 |
| neon_arm64         | arm64          |           1757.68 |          0.995313 |                         0.5   |         0.51375 |                 6 |                 3 |
| scalar_reference   | x86_64         |           1523.87 |          0.994375 |                         1     |         0.51375 |                 7 |                 4 |

## Policy counts by hardware

| hardware_profile   |   coherent_local |   guarded_fallback |   hybrid |   scalar |   simd |
|:-------------------|-----------------:|-------------------:|---------:|---------:|-------:|
| avx2_linux         |                8 |                  4 |        0 |        0 |     20 |
| avx512_linux       |                0 |                  0 |        0 |        0 |     32 |
| cloud_vm_baseline  |                8 |                  4 |        4 |        0 |     16 |
| neon_arm64         |                8 |                  4 |        4 |        0 |     16 |
| scalar_reference   |                8 |                  4 |       12 |        8 |      0 |

## Interpretation

- Policy portability measures whether the same structural signal produces stable decisions across architectures.
- AVX-style profiles should favor SIMD more often, while scalar and virtualized profiles penalize pressure differently.
- Low portability is not failure; it identifies architecture-specific constraints.
- This notebook creates a bridge from adaptive selection to architecture-aware scheduling.

## Next step

Notebook 10 can build a learned execution selector from the policy table and evaluate whether learned routing improves over transparent rules.

## Notebook 10 — Learned Execution Selector

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/10_learned_execution_selector.ipynb
)

# Report 10 — Learned Execution Selector

This report trains lightweight learned selectors for adaptive integer-serialization execution policy.

Constraint view:
> learned routing should improve policy selection without hiding the constraints that made the policy meaningful.

## Generated outputs

- Metrics CSV: <a href="results/notebook10_learned_execution_selector.csv">`results/notebook10_learned_execution_selector.csv`</a>
- Metrics JSON: <a href="results/notebook10_learned_execution_selector.json">`results/notebook10_learned_execution_selector.json`</a>
- Figure:  <a href="figures/notebook10_learned_policy_agreement.png">`figures/notebook10_learned_policy_agreement.png`</a>
- Figure:  <a href="figures/notebook10_feature_importance.png">`figures/notebook10_feature_importance.png`</a>
- Figure:  <a href="figures/notebook10_learned_gain_vs_rule.png">`figures/notebook10_learned_gain_vs_rule.png`</a>
- Figure:  <a href="figures/notebook10_policy_confusion_matrix.png">`figures/notebook10_policy_confusion_matrix.png`</a>
- Figure:  <a href="figures/notebook10_decision_tree.png">`figures/notebook10_decision_tree.png`</a>

## Summary

|   rows |   decision_tree_accuracy |   random_forest_accuracy |   learned_rule_agreement_rate |   mean_learned_gain_pct_vs_rule |
|-------:|-------------------------:|-------------------------:|------------------------------:|--------------------------------:|
|    150 |                 0.888889 |                 0.955556 |                          0.98 |                     -0.00792753 |

## Policy confusion table

| selected_policy   |   coherent_local |   guarded_fallback |   hybrid |   scalar |   simd |
|:------------------|-----------------:|-------------------:|---------:|---------:|-------:|
| coherent_local    |               30 |                  0 |        0 |        0 |      0 |
| guarded_fallback  |                0 |                 30 |        0 |        0 |      0 |
| hybrid            |                0 |                  0 |       48 |        0 |      0 |
| scalar            |                0 |                  0 |        0 |        6 |      0 |
| simd              |                0 |                  0 |        3 |        0 |     33 |

## Top feature importances

| feature                            |   importance |
|:-----------------------------------|-------------:|
| hardware_pressure_proxy            |   0.267224   |
| coherence_score                    |   0.214882   |
| truth_regime_clustered_ranges      |   0.134033   |
| truth_regime_uniform_32bit         |   0.107987   |
| truth_regime_low_entropy_repeating |   0.0953577  |
| truth_regime_sequential_ids        |   0.0534699  |
| hardware_profile_scalar_reference  |   0.0319537  |
| selected_efficiency                |   0.0297364  |
| truth_regime_zipfian_smallints     |   0.0286386  |
| hardware_profile_avx512_linux      |   0.0173409  |
| architecture_x86_64                |   0.00840921 |
| hardware_profile_avx2_linux        |   0.00336291 |
| architecture_arm64                 |   0.00232356 |
| architecture_virtualized           |   0.00201549 |
| hardware_profile_cloud_vm_baseline |   0.00184428 |

## Interpretation

- Learned selectors can recover policy structure from RML metrics and hardware-profile context.
- Feature importance helps verify whether the model is using meaningful constraints.
- Mismatches between learned and rule policies identify where hand-written heuristics may be brittle.
- This notebook bridges transparent constraint rules and learned runtime routing.

## Next step

Notebook 11 can simulate online distribution classification: detect regime changes from windows before selecting execution paths.

## Notebook 11 — Online Distribution Classification

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/11_online_distribution_classification.ipynb
)

# Report 11 — Online Distribution Classification

This report classifies streaming integer-distribution regimes before selecting execution policies.

Constraint view:
> adaptive execution depends on detecting structure before selecting policy.

## Generated outputs

- Metrics CSV: <a href="results/notebook11_online_distribution_classification.csv">`results/notebook11_online_distribution_classification.csv`</a>
- Metrics JSON: <a href="results/notebook11_online_distribution_classification.json">`results/notebook11_online_distribution_classification.json`</a>
- Figure: <a href="figures/notebook11_true_vs_predicted_regime_timeline.png">`figures/notebook11_true_vs_predicted_regime_timeline.png`</a>
- Figure: <a href="figures/notebook11_regime_confusion_matrix.png">`figures/notebook11_regime_confusion_matrix.png`</a>
- Figure: <a href="figures/notebook11_prediction_confidence_timeline.png">`figures/notebook11_prediction_confidence_timeline.png`</a>
- Figure: <a href="figures/notebook11_feature_importance.png">`figures/notebook11_feature_importance.png`</a>
- Figure: <a href="figures/notebook11_policy_correctness_from_regime.png">`figures/notebook11_policy_correctness_from_regime.png`</a>
- Figure: <a href="figures/notebook11_decision_tree.png">`figures/notebook11_decision_tree.png`</a>

## Summary

|   windows |   decision_tree_accuracy |   random_forest_accuracy |   online_regime_accuracy |   policy_correctness_from_regime |   mean_regime_probability |   mean_regime_confidence |
|----------:|-------------------------:|-------------------------:|-------------------------:|---------------------------------:|--------------------------:|-------------------------:|
|       160 |                        1 |                        1 |                        1 |                                1 |                  0.991198 |                 0.983698 |

## Regime → predicted policy table

| truth_regime          |   coherent_local |   guarded_fallback |   hybrid |   simd |
|:----------------------|-----------------:|-------------------:|---------:|-------:|
| clustered_ranges      |                0 |                 32 |        0 |      0 |
| low_entropy_repeating |               32 |                  0 |        0 |      0 |
| sequential_ids        |                0 |                  0 |       32 |      0 |
| uniform_32bit         |                0 |                  0 |        0 |     32 |
| zipfian_smallints     |                0 |                  0 |       32 |      0 |

## Feature importance

| feature                    |   importance |
|:---------------------------|-------------:|
| hardware_pressure_proxy    |    0.157598  |
| coherence_score            |    0.149127  |
| approx_entropy_bits        |    0.140855  |
| entropy_norm               |    0.128774  |
| branch_norm                |    0.101513  |
| repetition_ratio           |    0.0890698 |
| branch_pressure_score      |    0.0843426 |
| cache_window_reuse_proxy   |    0.0821167 |
| locality_small_delta_ratio |    0.0666037 |

## Interpretation

- Online distribution classification moves adaptive execution one step earlier than policy selection.
- Correct regime detection supports correct execution-path routing.
- Confidence traces show when the selector is stable and when it should hesitate.
- Feature importance checks whether the classifier relies on meaningful structure rather than arbitrary labels.

## Next step

Notebook 12 can analyze latency-throughput Pareto frontiers: choose policies under explicit tradeoff constraints.

## Notebook 12 — Latency Throughput Pareto Frontiers

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/12_latency_throughput_pareto_frontiers.ipynb)

# Report 12 — Latency / Throughput Pareto Frontiers

This report analyzes policy tradeoffs between throughput, latency, pressure, and coherence.

Constraint view:
> adaptive execution should not optimize one metric blindly; it should expose tradeoff frontiers.

## Generated outputs

- Candidate CSV: <a href="results/notebook12_pareto_candidates.csv">`results/notebook12_pareto_candidates.csv`</a>
- Candidate JSON: <a href="results/notebook12_pareto_candidates.json">`results/notebook12_pareto_candidates.json`</a>
- Selected policy CSV: <a href="results/notebook12_pareto_selected_policies.csv">`results/notebook12_pareto_selected_policies.csv`</a>
- Figure: <a href="figures/notebook12_latency_throughput_cloud.png">`figures/notebook12_latency_throughput_cloud.png`</a>
- Figure: <a href="figures/notebook12_pareto_policy_frequency.png`">`figures/notebook12_pareto_policy_frequency.png`</a>
- Figure: <a href="figures/notebook12_objective_policy_choices.png">`figures/notebook12_objective_policy_choices.png`</a>
- Figure: <a href="figures/notebook12_frontier_score_by_regime.png">`figures/notebook12_frontier_score_by_regime.png`</a>
- Figure: <a href="figures/notebook12_policy_timeline_by_objective.png">`figures/notebook12_policy_timeline_by_objective.png`</a>

## Summary

|   candidate_rows |   windows |   pareto_efficient_rows |   pareto_fraction |   objective_profiles |
|-----------------:|----------:|------------------------:|------------------:|---------------------:|
|              500 |       100 |                     160 |              0.32 |                    3 |

## Pareto-efficient policy frequency

| policy           |   pareto_count |
|:-----------------|---------------:|
| coherent_local   |             40 |
| simd             |             40 |
| hybrid           |             39 |
| scalar           |             21 |
| guarded_fallback |             20 |

## Selected policies by objective profile

| objective_profile   |   coherent_local |   guarded_fallback |   hybrid |   scalar |   simd |
|:--------------------|-----------------:|-------------------:|---------:|---------:|-------:|
| balanced_constraint |               33 |                 20 |       25 |        2 |     20 |
| latency_first       |               20 |                 20 |       31 |        1 |     28 |
| throughput_first    |               20 |                 20 |       26 |        0 |     34 |

## Interpretation

- Pareto frontiers expose which policies remain useful under multi-objective constraints.
- Throughput-first, latency-first, and balanced profiles can select different execution paths for the same window.
- Guarded fallback can be Pareto-efficient when pressure dominates fragmented regimes.
- SIMD is not universally optimal; it is optimal under objective profiles and regimes that reward wide throughput.

## Next step

Notebook 13 can introduce mixed-regime decomposition: classify windows that contain blends rather than single regimes.

## Notebook 13 — Mixed-Regime Decomposition

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/13_mixed_regime_decomposition.ipynb)

# Report 13 — Mixed-Regime Decomposition

This report estimates mixed-regime structure within streaming windows instead of forcing each window into one hard label.

Constraint view:
> real streams often contain mixed structure; adaptive runtimes should estimate mixture, not only classify.

## Generated outputs

- Metrics CSV: <a href="results/notebook13_mixed_regime_decomposition.csv">`results/notebook13_mixed_regime_decomposition.csv`</a>
- Metrics JSON: <a href="results/notebook13_mixed_regime_decomposition.json">`results/notebook13_mixed_regime_decomposition.json`</a>
- Figure: <a href="figures/notebook13_estimated_mixture_weights.png">`figures/notebook13_estimated_mixture_weights.png`</a>
- Figure: <a href="figures/notebook13_dominant_regime_timeline.png">`figures/notebook13_dominant_regime_timeline.png`</a>
- Figure: <a href="figures/notebook13_mixture_entropy_timeline.png">`figures/notebook13_mixture_entropy_timeline.png`</a>
- Figure: <a href="figures/notebook13_reconstruction_residuals.png">`figures/notebook13_reconstruction_residuals.png`</a>
- Figure: <a href="figures/notebook13_mixture_policy_timeline.png">`figures/notebook13_mixture_policy_timeline.png`</a>
- Figure: <a href="figures/notebook13_average_mixture_by_regime.png">`figures/notebook13_average_mixture_by_regime.png`</a>

## Summary

|   windows |   dominant_regime_accuracy |   mean_mixture_entropy |   mean_reconstruction_residual |   policy_changed_by_mixture_entropy_rate |
|----------:|---------------------------:|-----------------------:|-------------------------------:|-----------------------------------------:|
|       160 |                     0.7875 |                1.21063 |                      0.0401939 |                                  0.15625 |

## Mixture-aware policy counts

| policy           |   count |
|:-----------------|--------:|
| hybrid           |      79 |
| simd             |      32 |
| guarded_fallback |      29 |
| coherent_local   |      20 |

## Interpretation

- Mixture weights expose blended windows that hard classification hides.
- Mixture entropy identifies when runtime policy should hesitate or choose hybrid paths.
- Reconstruction residuals identify windows poorly explained by current prototypes.
- This notebook turns regime classification into regime decomposition.

## Next step

Notebook 14 can introduce online drift detection: detect when new or changing mixtures stop matching existing prototypes.

## Notebook 14 - Online Drift Detection

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/14_online_drift_detection.ipynb)

# Report 14 — Online Drift Detection

This report detects windows where existing mixed-regime prototypes no longer explain the stream.

Constraint view:
> when reconstruction residuals rise, existing execution prototypes no longer explain the stream.

## Generated outputs

- Metrics CSV: <a href="results/notebook14_online_drift_detection.csv">`results/notebook14_online_drift_detection.csv`</a>
- Metrics JSON: <a href="results/notebook14_online_drift_detection.json">`results/notebook14_online_drift_detection.json`</a>
- Episodes CSV: <a href="results/notebook14_drift_episodes.csv">`results/notebook14_drift_episodes.csv`</a>
- Figure: <a href="figures/notebook14_drift_score_timeline.png">`figures/notebook14_drift_score_timeline.png`</a>
- Figure: <a href="figures/notebook14_residual_entropy_zscores.png">`figures/notebook14_residual_entropy_zscores.png`</a>
- Figure: <a href="figures/notebook14_reconstruction_residual_alarms.png">`figures/notebook14_reconstruction_residual_alarms.png`</a>
- Figure: <a href="figures/notebook14_policy_regime_instability.png">`figures/notebook14_policy_regime_instability.png`</a>
- Figure: <a href="figures/notebook14_drift_alarm_timeline.png">`figures/notebook14_drift_alarm_timeline.png`</a>
- Figure: <a href="figures/notebook14_drift_episodes_summary.png">`figures/notebook14_drift_episodes_summary.png`</a>

## Summary

|   windows |   warning_windows |   alarm_windows |   unknown_candidate_windows |   drift_episodes |   max_drift_score |   mean_drift_score |   mean_reconstruction_residual |   mean_mixture_entropy |
|----------:|------------------:|----------------:|----------------------------:|-----------------:|------------------:|-------------------:|-------------------------------:|-----------------------:|
|       180 |                10 |               1 |                           4 |                1 |          0.673333 |           0.145204 |                      0.0514594 |                 1.1994 |

## Drift episodes

|   drift_episode_id |   start_window |   end_window |   duration |   max_drift_score |   mean_residual |   mean_entropy |   unknown_candidates |
|-------------------:|---------------:|-------------:|-----------:|------------------:|----------------:|---------------:|---------------------:|
|                  0 |            105 |          105 |          1 |          0.673333 |        0.162402 |        2.33424 |                    1 |

## Interpretation

- Reconstruction residuals measure prototype mismatch.
- Mixture entropy measures ambiguity in the regime decomposition.
- Policy and regime switch rates identify unstable runtime behavior.
- Drift alarms mark windows where adaptation should slow down, inspect, or propose new prototypes.

## Next step

Notebook 15 can perform prototype update and recovery: learn a new prototype from drift windows and test whether residuals decrease.

## Notebook 15 -

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/15_prototype_update_and_recovery.ipynb)

# Report 15 — Prototype Update and Recovery

This report updates the prototype bank using drift windows and tests whether reconstruction and drift alarms improve.

Constraint view:
> drift detection becomes useful when the runtime can recover by updating its prototype memory.

## Generated outputs

- Metrics CSV: <a href="results/notebook15_prototype_update_and_recovery.csv">`results/notebook15_prototype_update_and_recovery.csv`</a>
- Metrics JSON: <a href="results/notebook15_prototype_update_and_recovery.json">`results/notebook15_prototype_update_and_recovery.json`</a>
- Updated prototypes CSV: <a href="results/notebook15_updated_prototypes.csv">`results/notebook15_updated_prototypes.csv`</a>
- Figure: <a href="figures/notebook15_old_vs_new_residuals.png">`figures/notebook15_old_vs_new_residuals.png`</a>
- Figure: <a href="figures/notebook15_residual_reduction_timeline.png">`figures/notebook15_residual_reduction_timeline.png`</a>
- Figure: <a href="figures/notebook15_drift_score_before_after.pn">`figures/notebook15_drift_score_before_after.png`</a>
- Figure: <a href="figures/notebook15_learned_prototype_features.png">`figures/notebook15_learned_prototype_features.png`</a>
- Figure: <a href="figures/notebook15_updated_dominant_prototype_timeline.png">`figures/notebook15_updated_dominant_prototype_timeline.png`</a>
- Figure: <a href="figures/notebook15_alarm_recovery_summary.png">`figures/notebook15_alarm_recovery_summary.png`</a>

## Summary

|   windows |   drift_windows_used_for_prototype |   mean_old_residual |   mean_new_residual |   mean_residual_reduction |   mean_residual_reduction_pct |   old_alarm_count |   new_alarm_count |   recovered_alarm_count |   new_prototype_dominant_windows |
|----------:|-----------------------------------:|--------------------:|--------------------:|--------------------------:|------------------------------:|------------------:|------------------:|------------------------:|---------------------------------:|
|       180 |                                 31 |            0.467057 |            0.444465 |                 0.0225919 |                       8.27811 |                 1 |                 3 |                       0 |                               65 |

## Learned drift prototype

| regime                  |   entropy_norm |   repetition_ratio |   locality_small_delta_ratio |   cache_window_reuse_proxy |   branch_norm |   coherence_score |   hardware_pressure_proxy |
|:------------------------|---------------:|-------------------:|-----------------------------:|---------------------------:|--------------:|------------------:|--------------------------:|
| learned_drift_prototype |       0.509088 |           0.457231 |                     0.439492 |                   0.412959 |       0.64112 |          0.448685 |                  0.692703 |

## Updated policy counts

| policy             |   count |
|:-------------------|--------:|
| prototype_recovery |      65 |
| hybrid             |      38 |
| coherent_local     |      35 |
| simd               |      21 |
| guarded_fallback   |      21 |

## Interpretation

- A new prototype is learned from drift / unknown-candidate windows.
- Recovery is measured by comparing old and new reconstruction residuals.
- Alarm reduction indicates whether the new prototype explains previously anomalous windows.
- Windows dominated by the learned prototype become candidates for a new execution policy or deeper inspection.

## Next step

Notebook 16 can build a prototype memory bank with aging, pruning, and stability scores.

## Notebook 16 - Prototype Memory Bank Aging

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/16_prototype_memory_bank_aging.ipynb)

# Report 16 — Prototype Memory Bank Aging

This report manages an adaptive prototype memory bank using age, usage, residual quality, drift quality, and policy stability.

Constraint view:
> adaptive prototype memory needs both recall and forgetting.

## Generated outputs

- Window events CSV: <a href="results/notebook16_prototype_memory_bank_aging.csv">`results/notebook16_prototype_memory_bank_aging.csv`</a>
- Window events JSON: <a href="results/notebook16_prototype_memory_bank_aging.json">`results/notebook16_prototype_memory_bank_aging.json`</a>
- Memory summary CSV: <a href="results/notebook16_memory_bank_summary.csv">`results/notebook16_memory_bank_summary.csv`</a>
- Figure: <a href="figures/notebook16_memory_stability_scores.png">`figures/notebook16_memory_stability_scores.png`</a>
- Figure: <a href="figures/notebook16_effective_memory_weight.png">`figures/notebook16_effective_memory_weight.png`</a>
- Figure: <a href="figures/notebook16_prototype_usage_timeline.png">`figures/notebook16_prototype_usage_timeline.png`</a>
- Figure: <a href="figures/notebook16_memory_action_counts.png">`figures/notebook16_memory_action_counts.png`</a>
- Figure: <a href="figures/notebook16_memory_event_timeline.png">`figures/notebook16_memory_event_timeline.png`</a>
- Figure: <a href="figures/notebook16_memory_status_matrix.png">`figures/notebook16_memory_status_matrix.png`</a>

## Summary

|   windows |   prototype_count |   retain_count |   refresh_count |   watch_count |   prune_count |   mean_memory_stability_score |   mean_effective_memory_weight |
|----------:|------------------:|---------------:|----------------:|--------------:|--------------:|------------------------------:|-------------------------------:|
|       220 |                 6 |              4 |               2 |             0 |             0 |                      0.503969 |                       0.565477 |

## Memory bank summary

| prototype               |   first_seen |   last_seen |   age |   recency_gap |   usage_count |   usage_rate |   mean_residual |   mean_drift_score |   policy_switch_rate |   usage_score |   recency_score |   residual_quality_score |   drift_quality_score |   policy_stability_score |   memory_stability_score | memory_status   |   age_decay |   reinforced_weight |   effective_memory_weight | memory_action   |
|:------------------------|-------------:|------------:|------:|--------------:|--------------:|-------------:|----------------:|-------------------:|---------------------:|--------------:|----------------:|-------------------------:|----------------------:|-------------------------:|-------------------------:|:----------------|------------:|--------------------:|--------------------------:|:----------------|
| uniform_32bit           |            6 |         219 |   214 |             0 |            38 |     0.172727 |        0.472039 |          0.0745126 |            0.0263158 |      0.4      |        1        |               0.0422093  |              0.921377 |                 0.589474 |                 0.575596 | retain          |    1        |            0.79     |                  0.661358 | retain          |
| low_entropy_repeating   |           23 |         215 |   197 |             4 |            37 |     0.168182 |        0.480203 |          0.0616303 |            0.027027  |      0.366667 |        0.945946 |               0.0179647  |              1        |                 0.554955 |                 0.555575 | retain          |    0.923116 |            0.728359 |                  0.624689 | retain          |
| zipfian_smallints       |            0 |         214 |   220 |             5 |            34 |     0.154545 |        0.486252 |          0.0765296 |            0.0294118 |      0.266667 |        0.932432 |               0          |              0.909067 |                 0.439216 |                 0.49339  | retain          |    0.904837 |            0.681478 |                  0.568625 | retain          |
| learned_drift_prototype |           90 |         145 |   130 |            74 |            56 |     0.254545 |        0.149519 |          0.22548   |            0.0178571 |      1        |        0        |               1          |              0        |                 1        |                 0.6      | refresh         |    0.227638 |            0.497964 |                  0.559186 | refresh         |
| clustered_ranges        |            2 |         213 |   218 |             6 |            29 |     0.131818 |        0.483625 |          0.0749466 |            0.0344828 |      0.1      |        0.918919 |               0.00780244 |              0.918728 |                 0.193103 |                 0.41841  | retain          |    0.88692  |            0.611498 |                  0.495645 | retain          |
| sequential_ids          |            3 |         218 |   217 |             1 |            26 |     0.118182 |        0.47962  |          0.0831649 |            0.0384615 |      0        |        0.986486 |               0.0196962  |              0.868571 |                 0        |                 0.380846 | refresh         |    0.980199 |            0.637129 |                  0.48336  | refresh         |

## Interpretation

- Retained prototypes explain recent windows with stable residuals and useful policy behavior.
- Refresh candidates are useful but unstable, suggesting prototype update or split.
- Watch candidates are low-confidence memory entries that should not be removed immediately.
- Prune candidates are unused or stale prototypes with weak effective memory weight.

## Next step

Notebook 17 can build hierarchical prototype trees: split broad prototypes into child prototypes and merge redundant ones.

## Notebook 17 — Hierarchical Prototype Routing

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/17_hierarchical_prototype_routing.ipynb)

# Report 17 — Hierarchical Prototype Routing

This report turns the prototype memory bank into a coarse-to-fine routing hierarchy.

Constraint view:
> adaptive runtimes should route from broad structure to precise execution behavior.

## Generated outputs

- Window routes CSV: <a href="results/notebook17_hierarchical_prototype_routing.csv">`results/notebook17_hierarchical_prototype_routing.csv`</a>
- Window routes JSON: <a href="results/notebook17_hierarchical_prototype_routing.json">`results/notebook17_hierarchical_prototype_routing.json`</a>
- Prototype hierarchy CSV: <a href="results/notebook17_prototype_hierarchy.csv">`results/notebook17_prototype_hierarchy.csv`</a>
- Parent route summary CSV: <a href="esults/notebook17_parent_route_summary.csv">`esults/notebook17_parent_route_summary.csv`</a>
- Merge candidates CSV: <a href="results/notebook17_merge_candidates.csv">`results/notebook17_merge_candidates.csv`</a>
- Split candidates CSV: <a href="results/notebook17_split_candidates.csv">`results/notebook17_split_candidates.csv`</a>
- Figure: <a href="figures/notebook17_prototype_hierarchy_projection.png">`figures/notebook17_prototype_hierarchy_projection.png`</a>
- Figure: <a href="figures/notebook17_parent_route_timeline.png">`figures/notebook17_parent_route_timeline.png`</a>
- Figure: <a href="figures/notebook17_child_prototype_timeline.png">`figures/notebook17_child_prototype_timeline.png`</a>
- Figure: <a href="figures/notebook17_route_switch_rates.png">`figures/notebook17_route_switch_rates.png`</a>
- Figure: <a href="figures/notebook17_hierarchical_stability_timeline.png">`figures/notebook17_hierarchical_stability_timeline.png`</a>
- Figure: <a href="figures/notebook17_parent_route_summary_matrix.png">`figures/notebook17_parent_route_summary_matrix.png`</a>
- Figure: <a href="figures/notebook17_merge_split_counts.png">`figures/notebook17_merge_split_counts.png`</a>

## Summary

|   windows |   prototype_count |   parent_route_count |   mean_hierarchical_stability |   mean_parent_switch_rate |   mean_child_switch_rate |   mean_policy_switch_rate |   merge_candidate_count |   split_candidate_count |
|----------:|------------------:|---------------------:|------------------------------:|--------------------------:|-------------------------:|--------------------------:|------------------------:|------------------------:|
|       220 |                 6 |                    3 |                      0.495276 |                  0.414248 |                   0.6152 |                    0.5652 |                       5 |                       3 |

## Parent route summary

| parent_route   |   prototype_count |   mean_memory_stability |   mean_effective_weight |   total_usage |   mean_entropy |   mean_pressure |   mean_coherence |
|:---------------|------------------:|------------------------:|------------------------:|--------------:|---------------:|----------------:|-----------------:|
| parent_0       |                 4 |                  0.5225 |                  0.5725 |           146 |           0.56 |          0.8175 |           0.2825 |
| parent_1       |                 1 |                  0.38   |                  0.48   |            22 |           0.9  |          0.25   |           0.55   |
| parent_2       |                 1 |                  0.56   |                  0.62   |            32 |           0.1  |          0.02   |           0.95   |

## Merge candidates

| prototype_a       | prototype_b             | parent_route   |   feature_distance | merge_candidate   |
|:------------------|:------------------------|:---------------|-------------------:|:------------------|
| uniform_32bit     | zipfian_smallints       | parent_0       |           1.28413  | False             |
| uniform_32bit     | clustered_ranges        | parent_0       |           1.08991  | True              |
| uniform_32bit     | learned_drift_prototype | parent_0       |           0.996393 | True              |
| zipfian_smallints | clustered_ranges        | parent_0       |           0.698498 | True              |
| zipfian_smallints | learned_drift_prototype | parent_0       |           0.514976 | True              |
| clustered_ranges  | learned_drift_prototype | parent_0       |           0.897274 | True              |

## Split candidates

| prototype               | parent_route   |   memory_stability_score |   usage_count |   windows |   mean_child_switch |   mean_parent_switch |   mean_stability | split_candidate   |
|:------------------------|:---------------|-------------------------:|--------------:|----------:|--------------------:|---------------------:|-----------------:|:------------------|
| low_entropy_repeating   | parent_2       |                     0.56 |            32 |        37 |            0.770721 |             0.544144 |         0.353739 | True              |
| sequential_ids          | parent_1       |                     0.38 |            22 |        20 |            0.816667 |             0.54625  |         0.333208 | True              |
| uniform_32bit           | parent_0       |                     0.58 |            36 |        36 |            0.785185 |             0.486243 |         0.379471 | False             |
| zipfian_smallints       | parent_0       |                     0.49 |            26 |        38 |            0.799237 |             0.531032 |         0.349252 | True              |
| clustered_ranges        | parent_0       |                     0.42 |            20 |        31 |            0.786766 |             0.51373  |         0.362225 | False             |
| learned_drift_prototype | parent_0       |                     0.6  |            64 |        58 |            0.128736 |             0.111494 |         0.880115 | False             |

## Interpretation

- Parent routes provide stable coarse structure even when child prototypes switch.
- Child routes preserve precision for execution-policy choice.
- Merge candidates identify redundant prototypes within a parent group.
- Split candidates identify broad or unstable prototypes that may need refinement.

## Next step

Notebook 18 can perform prototype compression: merge redundant prototypes while preserving reconstruction quality.

## Notebook 18 — Hierarchical Drift Forecasting

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/18_hierarchical_drift_forecasting.ipynb)

# Report 18 — Hierarchical Drift Forecasting

This report forecasts parent-route transitions and identifies early-warning windows before hierarchical drift.

Constraint view:
> hierarchical routing becomes more useful when route pressure can be forecast before drift alarms fire.

## Generated outputs

- Forecasting CSV: <a href="results/notebook18_hierarchical_drift_forecasting.csv">`results/notebook18_hierarchical_drift_forecasting.csv`</a>
- Forecasting JSON: <a href="results/notebook18_hierarchical_drift_forecasting.json">`results/notebook18_hierarchical_drift_forecasting.json`</a>
- Transition matrix CSV: <a href="results/notebook18_parent_transition_matrix.csv">`results/notebook18_parent_transition_matrix.csv`</a>
- Transition warning evaluation CSV: <a href="results/notebook18_transition_warning_eval.csv">`results/notebook18_transition_warning_eval.csv`</a>
- Figure: <a href="figures/notebook18_parent_transition_matrix.png">`figures/notebook18_parent_transition_matrix.png`</a>
- Figure: <a href="figures/notebook18_route_pressure_timeline.png">`figures/notebook18_route_pressure_timeline.png`</a>
- Figure: <a href="figures/notebook18_early_warning_score_timeline.png">`figures/notebook18_early_warning_score_timeline.png`</a>
- Figure: <a href="figures/notebook18_forecast_correctness_timeline.png">`figures/notebook18_forecast_correctness_timeline.png`</a>
- Figure: <a href="figures/notebook18_parent_route_actual_vs_forecast.png">`figures/notebook18_parent_route_actual_vs_forecast.png`</a>
- Figure: <a href="figures/notebook18_transition_warning_recall.png">`figures/notebook18_transition_warning_recall.png`</a>
- Figure: <a href="figures/notebook18_parent_persistence_by_route.png">`figures/notebook18_parent_persistence_by_route.png`</a>

## Summary

|   windows |   parent_route_count |   forecast_accuracy |   early_warning_threshold |   early_warning_windows |   parent_transition_count |   early_warning_recall_prior_horizon |   forecast_horizon |   mean_route_pressure |   mean_route_uncertainty |
|----------:|---------------------:|--------------------:|--------------------------:|------------------------:|--------------------------:|-------------------------------------:|-------------------:|----------------------:|-------------------------:|
|       240 |                    3 |            0.878661 |                  0.376667 |                      40 |                        30 |                             0.586207 |                  8 |              0.121366 |                 0.121366 |

## Parent transition probabilities

|          |   parent_0 |   parent_1 |   parent_2 |
|:---------|-----------:|-----------:|-----------:|
| parent_0 |  0.903614  |  0.0481928 |  0.0481928 |
| parent_1 |  0.0285714 |  0.857143  |  0.114286  |
| parent_2 |  0.0581395 |  0.0697674 |  0.872093  |

## Parent transition warning evaluation

|   transition_window | from_parent   | to_parent   | warning_in_prior_horizon   |   max_prior_warning_score |   horizon |
|--------------------:|:--------------|:------------|:---------------------------|--------------------------:|----------:|
|                  17 | parent_0      | parent_2    | False                      |                 0.035     |         8 |
|                  18 | parent_2      | parent_0    | True                       |                 0.39407   |         8 |
|                  26 | parent_0      | parent_2    | False                      |                 0.163333  |         8 |
|                  27 | parent_2      | parent_0    | True                       |                 0.407403  |         8 |
|                  50 | parent_0      | parent_2    | False                      |                 0         |         8 |
|                  51 | parent_2      | parent_0    | True                       |                 0.39407   |         8 |
|                  55 | parent_0      | parent_1    | True                       |                 0.39407   |         8 |
|                  66 | parent_1      | parent_2    | True                       |                 0.386667  |         8 |
|                  67 | parent_2      | parent_1    | True                       |                 0.386667  |         8 |
|                  72 | parent_1      | parent_2    | True                       |                 0.536667  |         8 |
|                  73 | parent_2      | parent_1    | True                       |                 0.536667  |         8 |
|                  80 | parent_1      | parent_2    | True                       |                 0.526667  |         8 |
|                  81 | parent_2      | parent_0    | True                       |                 0.587403  |         8 |
|                  82 | parent_0      | parent_2    | True                       |                 0.587403  |         8 |
|                 115 | parent_2      | parent_0    | False                      |                 0.237403  |         8 |
|                 131 | parent_0      | parent_1    | False                      |                 0.0233333 |         8 |
|                 132 | parent_1      | parent_0    | True                       |                 0.506667  |         8 |
|                 145 | parent_0      | parent_1    | False                      |                 0.0133333 |         8 |
|                 158 | parent_1      | parent_2    | False                      |                 0.373333  |         8 |
|                 159 | parent_2      | parent_1    | True                       |                 0.417403  |         8 |
|                 170 | parent_1      | parent_0    | False                      |                 0.363333  |         8 |
|                 171 | parent_0      | parent_1    | False                      |                 0.363333  |         8 |
|                 190 | parent_1      | parent_2    | False                      |                 0.363333  |         8 |
|                 198 | parent_2      | parent_1    | True                       |                 0.560736  |         8 |
|                 199 | parent_1      | parent_2    | True                       |                 0.53      |         8 |
|                 220 | parent_2      | parent_1    | False                      |                 0.250736  |         8 |
|                 221 | parent_1      | parent_2    | True                       |                 0.506667  |         8 |
|                 231 | parent_2      | parent_1    | False                      |                 0.250736  |         8 |
|                 232 | parent_1      | parent_2    | True                       |                 0.52      |         8 |

## Interpretation

- Parent-route persistence estimates which coarse routing states are stable.
- Route pressure estimates likelihood that the current parent route will change.
- Early-warning windows flag route instability before parent-route transitions.
- Forecast confidence distinguishes stable routing from uncertain transition regions.

## The current sequence (14–18) now reads like a coherent research arc:

14 → online drift detection
15 → adaptive prototype updating
16 → memory-bank stabilization
17 → hierarchical routing
18 → hierarchical forecasting + early warning

That’s a real systems story now, not isolated notebooks.

## Next step

Notebook 19 can perform recursive memory compression: merge redundant prototypes while preserving routing and reconstruction quality.

## Notebook 19 - Recursive Memory Compression

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/19_recursive_memory_compression.ipynb)

# Report 19 — Recursive Memory Compression

This report compresses prototype memory into macro-prototypes while tracking route stability and reconstruction proxy quality.

Constraint view:
> adaptive memory should preserve useful distinctions while compressing redundant structure.

## Generated outputs

- Window compression CSV: <a href="results/notebook19_recursive_memory_compression.csv">`results/notebook19_recursive_memory_compression.csv`</a>
- Window compression JSON: <a href="results/notebook19_recursive_memory_compression.json">`results/notebook19_recursive_memory_compression.json`</a>
- Prototype to macro map CSV: <a href="results/notebook19_prototype_to_macro_map.csv">`results/notebook19_prototype_to_macro_map.csv`</a>
- Compressed macro-prototypes CSV: <a href="results/notebook19_compressed_macro_prototypes.csv">`results/notebook19_compressed_macro_prototypes.csv`</a>
- Redundancy pairs CSV: <a href="results/notebook19_redundancy_pairs.csv">`results/notebook19_redundancy_pairs.csv`</a>
- Macro transition matrix CSV: <a href="results/notebook19_macro_transition_matrix.csv">`results/notebook19_macro_transition_matrix.csv`</a>
- Figure: <a href="figures/notebook19_prototype_macro_projection.png">`figures/notebook19_prototype_macro_projection.png`</a>
- Figure: <a href="figures/notebook19_compression_ratio_summary.png">`figures/notebook19_compression_ratio_summary.png`</a>
- Figure: <a href="figures/notebook19_macro_route_timeline.png">`figures/notebook19_macro_route_timeline.png`</a>
- Figure: <a href="figures/notebook19_child_vs_macro_switch_rates.png">`figures/notebook19_child_vs_macro_switch_rates.png`</a>
- Figure: <a href="figures/notebook19_compressed_route_stability.png">`figures/notebook19_compressed_route_stability.png`</a>
- Figure: <a href="figures/notebook19_compression_residual_by_prototype.png">`figures/notebook19_compression_residual_by_prototype.png`</a>
- Figure: <a href="figures/notebook19_macro_transition_matrix.png">`figures/notebook19_macro_transition_matrix.png`</a>

## Summary

|   windows |   original_prototype_count |   compressed_macro_count |   compression_ratio |   mean_compression_residual |   mean_compression_quality |   mean_macro_switch_rate |   mean_child_switch_rate |   mean_compressed_route_stability |   merge_recommended_pairs |
|----------:|---------------------------:|-------------------------:|--------------------:|----------------------------:|---------------------------:|-------------------------:|-------------------------:|----------------------------------:|--------------------------:|
|       240 |                          6 |                        5 |            0.166667 |                    0.109527 |                   0.912916 |                 0.609759 |                 0.609759 |                          0.398686 |                         5 |

## Compressed macro-prototypes

| macro_prototype   | members                                    |   member_count |   total_usage_count |   mean_memory_stability |   mean_effective_memory_weight |   entropy_norm |   repetition_ratio |   locality_small_delta_ratio |   cache_window_reuse_proxy |   branch_norm |   coherence_score |   hardware_pressure_proxy |
|:------------------|:-------------------------------------------|---------------:|--------------------:|------------------------:|-------------------------------:|---------------:|-------------------:|-----------------------------:|---------------------------:|--------------:|------------------:|--------------------------:|
| macro_0           | zipfian_smallints, learned_drift_prototype |              2 |                  90 |                   0.545 |                          0.565 |        0.34354 |           0.631504 |                     0.354248 |                   0.379735 |      0.680354 |          0.434867 |                  0.705133 |
| macro_1           | clustered_ranges                           |              1 |                  20 |                   0.42  |                          0.5   |        0.55    |           0.98     |                     0        |                   0.01     |      0.79     |          0.18     |                  0.98     |
| macro_2           | low_entropy_repeating                      |              1 |                  32 |                   0.56  |                          0.62  |        0.1     |           0.98     |                     1        |                   0.94     |      0.02     |          0.95     |                  0.02     |
| macro_3           | sequential_ids                             |              1 |                  22 |                   0.38  |                          0.48  |        0.9     |           0        |                     1        |                   0        |      0.2      |          0.55     |                  0.25     |
| macro_4           | uniform_32bit                              |              1 |                  36 |                   0.58  |                          0.66  |        1       |           0        |                     0        |                   0        |      0.72     |          0.08     |                  0.88     |

## Prototype → macro map

| regime                  | macro_prototype   |   compression_residual |   compression_quality |
|:------------------------|:------------------|-----------------------:|----------------------:|
| low_entropy_repeating   | macro_2           |               0        |              1        |
| sequential_ids          | macro_3           |               0        |              1        |
| uniform_32bit           | macro_4           |               0        |              1        |
| zipfian_smallints       | macro_0           |               0.255209 |              0.79668  |
| clustered_ranges        | macro_1           |               0        |              1        |
| learned_drift_prototype | macro_0           |               0.259767 |              0.793798 |

## Top redundancy pairs

| prototype_a           | prototype_b             |   feature_distance |   feature_similarity | same_policy   |   usage_balance |   redundancy_score | merge_recommended   |
|:----------------------|:------------------------|-------------------:|---------------------:|:--------------|----------------:|-------------------:|:--------------------|
| zipfian_smallints     | learned_drift_prototype |           0.514976 |             0.783166 | False         |        0.958333 |           0.652808 | True                |
| zipfian_smallints     | clustered_ranges        |           0.698498 |             0.705892 | False         |        0.941667 |           0.60008  | True                |
| sequential_ids        | zipfian_smallints       |           1.52381  |             0.358388 | True          |        0.916667 |           0.570452 | True                |
| clustered_ranges      | learned_drift_prototype |           0.897274 |             0.622196 | False         |        0.9      |           0.539428 | True                |
| uniform_32bit         | learned_drift_prototype |           0.996393 |             0.580461 | False         |        0.954167 |           0.520425 | True                |
| uniform_32bit         | clustered_ranges        |           1.08991  |             0.541086 | False         |        0.945833 |           0.493581 | False               |
| sequential_ids        | learned_drift_prototype |           1.11472  |             0.530639 | False         |        0.875    |           0.476165 | False               |
| uniform_32bit         | zipfian_smallints       |           1.28413  |             0.459306 | False         |        0.995833 |           0.447924 | False               |
| sequential_ids        | uniform_32bit           |           1.37775  |             0.419888 | False         |        0.920833 |           0.411052 | False               |
| low_entropy_repeating | zipfian_smallints       |           1.47672  |             0.378217 | False         |        0.954167 |           0.388966 | False               |
| low_entropy_repeating | learned_drift_prototype |           1.45475  |             0.387467 | False         |        0.9125   |           0.388728 | False               |
| low_entropy_repeating | sequential_ids          |           1.65206  |             0.304388 | False         |        0.9625   |           0.342227 | False               |

## Interpretation

- Macro-prototypes reduce memory footprint while preserving coarse routing behavior.
- Compression residual marks prototypes that lose detail under merging.
- Macro switch rate should be lower than child switch rate if compression stabilizes routing.
- Redundancy pairs identify candidates for safe merge or grouped monitoring.

## (Notebooks 17-19)

The plateau structures in 17–19 especially suggest a clean next step into:
“stable latent manifolds under recursive routing pressure.”

## Next step

Notebook 20 can add CGCS-style constraint gating over compressed route memory.

#Notebook 20 - Constraint-Gated Macro Routing

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/20_constraint_gated_macro_routing.ipynb)

# Report 20 — Constraint-Gated Macro Routing

This report adds a CGCS-style constraint gate over compressed macro routes.

Constraint view:
> compressed memory is useful only when macro routes remain coherent, stable, and recoverable under observed pressure.

## Generated outputs

- Constraint-gated routing CSV: <a href="results/notebook20_constraint_gated_macro_routing.csv">`results/notebook20_constraint_gated_macro_routing.csv`</a>
- Constraint-gated routing JSON: <a href="results/notebook20_constraint_gated_macro_routing.json">`results/notebook20_constraint_gated_macro_routing.json`</a>
- Gate summary CSV: <a href="results/notebook20_gate_summary_by_macro_route.csv">`results/notebook20_gate_summary_by_macro_route.csv`</a>
- Gate transition matrix CSV: <a href="results/notebook20_gate_transition_matrix.csv">`results/notebook20_gate_transition_matrix.csv`</a>
- Figure: <a href="/figures/notebook20_macro_cgcs_score_timeline.png">`/figures/notebook20_macro_cgcs_score_timeline.png`</a>
- Figure: <a href="figures/notebook20_constraint_gate_timeline.png">`figures/notebook20_constraint_gate_timeline.png`</a>
- Figure: <a href="figures/notebook20_gate_summary_by_macro_route.png">`figures/notebook20_gate_summary_by_macro_route.png`</a>
- Figure: <a href="figures/notebook20_component_matrix_by_macro_route.png">`figures/notebook20_component_matrix_by_macro_route.png`</a>
- Figure: <a href="figures/notebook20_raw_vs_gated_switch_rates.png">`figures/notebook20_raw_vs_gated_switch_rates.png`</a>
- Figure: <a href="figures/notebook20_decompression_recommendation_timeline.png">`figures/notebook20_decompression_recommendation_timeline.png`</a>
- Figure: <a href="figures/notebook20_gate_transition_matrix.png">`figures/notebook20_gate_transition_matrix.png`</a>
- Figure: <a href="figures/notebook20_gated_stability_score.png">`figures/notebook20_gated_stability_score.png`</a>

## Summary

|   windows |   macro_route_count |   mean_macro_cgcs_score |   accepted_windows |   watch_windows |   fallback_windows |   decompression_recommended_windows |   mean_raw_macro_switch_rate |   mean_gated_macro_switch_rate |   mean_gated_stability_score |
|----------:|--------------------:|------------------------:|-------------------:|----------------:|-------------------:|------------------------------------:|-----------------------------:|-------------------------------:|-----------------------------:|
|       240 |                   5 |                 0.54585 |                 12 |             155 |                 73 |                                  94 |                      0.62587 |                       0.621981 |                      0.49202 |

## Gate summary by macro route

| macro_route   |   windows |   mean_cgcs |   min_cgcs |   accepted_rate |   watch_rate |   fallback_rate |   decompression_rate |   mean_residual |   mean_stability |   mean_pressure |
|:--------------|----------:|------------:|-----------:|----------------:|-------------:|----------------:|---------------------:|----------------:|-----------------:|----------------:|
| macro_4       |        32 |    0.600429 |   0.420215 |       0.09375   |     0.8125   |        0.09375  |             0.21875  |        0.11187  |         0.471002 |        0.614983 |
| macro_3       |        46 |    0.549579 |   0.368707 |       0         |     0.826087 |        0.173913 |             0.282609 |        0.131436 |         0.427586 |        0.520706 |
| macro_0       |        87 |    0.547009 |   0.296799 |       0.0804598 |     0.551724 |        0.367816 |             0.471264 |        0.128647 |         0.729734 |        0.225362 |
| macro_2       |        47 |    0.538734 |   0.305138 |       0.0425532 |     0.680851 |        0.276596 |             0.340426 |        0.112703 |         0.450255 |        0.467122 |
| macro_1       |        28 |    0.485691 |   0.366511 |       0         |     0.392857 |        0.607143 |             0.607143 |        0.132053 |         0.477034 |        0.611385 |

## Gate transition probabilities

|          |   accepted |    watch |   fallback |
|:---------|-----------:|---------:|-----------:|
| accepted |  0.0833333 | 0.666667 |   0.25     |
| watch    |  0.0516129 | 0.658065 |   0.290323 |
| fallback |  0.0416667 | 0.625    |   0.333333 |

## Interpretation

- Accepted macro routes are compressed states that remain coherent under route pressure.
- Watch routes are retained but monitored for instability.
- Fallback routes are blocked or rerouted because compression is not currently trustworthy.
- Decompression recommendations identify when recursive compression should temporarily expand.

## Next step

Notebook 21 can build graph-based route topology over accepted, watch, and fallback macro states.

## Notebook 21 -

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/21_adaptive_threshold_gating.ipynb)

# Report 21 — Adaptive Threshold Gating

This report replaces fixed CGCS-style macro-route gates with adaptive thresholds.

Constraint view:
> macro-routing gates should tighten under instability and relax under stable coherent plateaus.

## Generated outputs

- Adaptive threshold gating CSV: <a href="results/notebook21_adaptive_threshold_gating.csv">`results/notebook21_adaptive_threshold_gating.csv`</a>
- Adaptive threshold gating JSON: <a href="results/notebook21_adaptive_threshold_gating.json">`results/notebook21_adaptive_threshold_gating.json`</a>
- Fixed vs adaptive summary JSON: <a href="results/notebook21_fixed_vs_adaptive_summary.json">`results/notebook21_fixed_vs_adaptive_summary.json`</a>
- Fixed gate transition matrix CSV: <a href="results/notebook21_fixed_gate_transition_matrix.csv">`results/notebook21_fixed_gate_transition_matrix.csv`</a>
- Adaptive gate transition matrix CSV: <a href="results/notebook21_adaptive_gate_transition_matrix.csv">`results/notebook21_adaptive_gate_transition_matrix.csv`</a>
- Figure: <a href="figures/notebook21_adaptive_threshold_timeline.png">`/figures/notebook21_adaptive_threshold_timeline.png`</a>
- Figure: <a href="figures/notebook21_fixed_vs_adaptive_gate_timeline.png">`/figures/notebook21_fixed_vs_adaptive_gate_timeline.png`</a>
- Figure: <a href="figures/notebook21_early_decompression_candidates.png">`/figures/notebook21_early_decompression_candidates.png`</a>
- Figure: <a href="figures/notebook21_fixed_vs_adaptive_switch_rates.png">`/figures/notebook21_fixed_vs_adaptive_switch_rates.png`</a>
- Figure: <a href="figures/notebook21_fixed_vs_adaptive_stability.png">`/figures/notebook21_fixed_vs_adaptive_stability.png`</a>
- Figure: <a href="figures/notebook21_threshold_pressure_components.png">`/figures/notebook21_threshold_pressure_components.png`</a>
- Figure: <a href="figures/notebook21_adaptive_gate_transition_matrix.png">`/figures/notebook21_adaptive_gate_transition_matrix.png`</a>
- Figure: <a href="figures/notebook21_fixed_vs_adaptive_gate_counts.png">`/figures/notebook21_fixed_vs_adaptive_gate_counts.png`</a>

## Summary

|   windows |   fixed_accepted |   adaptive_accepted |   fixed_watch |   adaptive_watch |   fixed_fallback |   adaptive_fallback |   early_decompression_candidates |   mean_fixed_switch_rate |   mean_adaptive_switch_rate |   mean_fixed_stability_score |   mean_adaptive_stability_score |
|----------:|-----------------:|--------------------:|--------------:|-----------------:|-----------------:|--------------------:|---------------------------------:|-------------------------:|----------------------------:|-----------------------------:|--------------------------------:|
|       240 |               24 |                  30 |           144 |              117 |               72 |                  93 |                              114 |                 0.612583 |                    0.543447 |                     0.506821 |                        0.498009 |

## Adaptive gate transition probabilities

|          |   accepted |    watch |   fallback |
|:---------|-----------:|---------:|-----------:|
| accepted |  0.833333  | 0.166667 |   0        |
| watch    |  0.0431034 | 0.594828 |   0.362069 |
| fallback |  0         | 0.451613 |   0.548387 |

## Fixed gate transition probabilities

|          |   accepted |    watch |   fallback |
|:---------|-----------:|---------:|-----------:|
| accepted |  0.5       | 0.416667 |  0.0833333 |
| watch    |  0.0769231 | 0.65035  |  0.272727  |
| fallback |  0.0138889 | 0.555556 |  0.430556  |

## Interpretation

- Adaptive gates tighten during high pressure, high volatility, and high residual windows.
- Adaptive gates relax during stable compressed-route plateaus.
- Early decompression candidates identify windows where macro compression may need temporary expansion.
- Fixed gates are easier to interpret; adaptive gates are better for streaming pressure-sensitive routing.

## Next step

Notebook 22 can build predictive decompression: forecast decompression before fallback occurs.

## Notebook 22

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/22_predictive_decompression_forecasting.ipynb)


# Report 22 — Predictive Decompression Forecasting

Notebook 22 forecasts decompression events before fallback occurs.

Constraint view:
> recursive compression should expand before coherence collapse propagates through compressed macro routes.

## Generated outputs

- Predictive decompression CSV: <a href="results/notebook22_predictive_decompression.csv">`results/notebook22_predictive_decompression.csv`</a>
- Predictive decompression JSON: <a href="results/notebook22_predictive_decompression.json">`results/notebook22_predictive_decompression.json`</a>
- Summary CSV: <a href="results/notebook22_summary.csv">`results/notebook22_summary.csv`</a>
- Feature importance CSV: <a href="results/notebook22_feature_importance.csv">`results/notebook22_feature_importance.csv`</a>
- Forecast transition matrix CSV: <a href="results/notebook22_forecast_transition_matrix.csv">`results/notebook22_forecast_transition_matrix.csv`</a>
- Classification report CSV: <a href="results/notebook22_classification_report.csv">`results/notebook22_classification_report.csv`</a>
- Confusion matrix CSV: <a href="results/notebook22_confusion_matrix.csv">`results/notebook22_confusion_matrix.csv`</a>
- Figure: <a href="figures/notebook22_probability_timeline.png">`figures/notebook22_probability_timeline.png`</a>
- Figure: <a href="figures/notebook22_actual_vs_predicted.png">`figures/notebook22_actual_vs_predicted.png`</a>
- Figure: <a href="figures/notebook22_feature_importance.png">`figures/notebook22_feature_importance.png`</a>
- Figure: <a href="figures/notebook22_forecast_transition_matrix.png">`figures/notebook22_forecast_transition_matrix.png`</a>
- Figure: <a href="figures/notebook22_roc_curve.png">`figures/notebook22_roc_curve.png`</a>
- Figure: <a href="figures/notebook22_pressure_vs_probability.png">`figures/notebook22_pressure_vs_probability.png`</a>
- Figure: <a href="figures/notebook22_pca_projection.png">`figures/notebook22_pca_projection.png`</a>

## Summary

|   windows |   forecast_horizon |   forecast_positive_windows |   actual_future_decompression_windows |   roc_auc |   mean_probability |   mean_pressure |   mean_stability |   mean_switch_rate |
|----------:|-------------------:|----------------------------:|--------------------------------------:|----------:|-------------------:|----------------:|-----------------:|-------------------:|
|       240 |                  5 |                          42 |                                    52 |  0.943433 |           0.209657 |        0.460494 |         0.574643 |           0.759015 |

## Feature importance

| feature             |   importance |
|:--------------------|-------------:|
| rolling_pressure    |    0.239532  |
| rolling_residual    |    0.189588  |
| macro_cgcs_score    |    0.189084  |
| rolling_stability   |    0.166333  |
| rolling_volatility  |    0.123643  |
| rolling_switch_rate |    0.0641996 |
| macro_route_id      |    0.0276208 |

## Forecast transition probabilities

|          |   stable |   forecast |
|:---------|---------:|-----------:|
| stable   | 0.954315 |  0.0456853 |
| forecast | 0.214286 |  0.785714  |

## Classification report

|              |   precision |   recall |   f1-score |   support |
|:-------------|------------:|---------:|-----------:|----------:|
| 0            |    0.929293 | 0.978723 |   0.953368 |   188     |
| 1            |    0.904762 | 0.730769 |   0.808511 |    52     |
| accuracy     |    0.925    | 0.925    |   0.925    |     0.925 |
| macro avg    |    0.917027 | 0.854746 |   0.880939 |   240     |
| weighted avg |    0.923978 | 0.925    |   0.921982 |   240     |

## Interpretation

- Forecast probability estimates decompression risk before fallback emerges.
- Rolling pressure, residual instability, and switch-rate volatility dominate decompression forecasting.
- Stable compressed plateaus reduce decompression probability.
- Pressure spikes increase forecast instability and future decompression likelihood.
- Forecast transition structure reveals persistence between stable and unstable routing phases.

## Next step

Notebook 23 can build multi-horizon forecasting:
- short horizon,
- medium horizon,
- long horizon,
- recursive decompression cascade prediction.

## Notebook 23

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/23_predictive_constraint_routing.ipynb)

# Report 23 — Predictive Constraint Routing

Notebook 23 uses decompression forecasts to route before fallback occurs.

Constraint view:
> predictive routing is useful when forecast risk can stabilize compressed routes before fallback becomes necessary.

## Generated outputs

- Predictive constraint routing CSV: <a href="results/notebook23_predictive_constraint_routing.csv">`results/notebook23_predictive_constraint_routing.csv`</a>
- Predictive constraint routing JSON: <a href="results/notebook23_predictive_constraint_routing.json">`results/notebook23_predictive_constraint_routing.json`</a>
- Summary CSV: <a href="results/notebook23_summary.csv">`results/notebook23_summary.csv`</a>
- Action summary CSV: <a href="results/notebook23_action_summary.csv">`results/notebook23_action_summary.csv`</a>
- Route summary CSV: <a href="results/notebook23_route_summary.csv">`results/notebook23_route_summary.csv`</a>
- Feature importance CSV: <a href="results/notebook23_feature_importance.csv">`results/notebook23_feature_importance.csv`</a>
- Predictive gate transition matrix CSV: <a href="results/notebook23_predictive_gate_transition_matrix.csv">`results/notebook23_predictive_gate_transition_matrix.csv`</a>
- Reactive gate transition matrix CSV: <a href="results/notebook23_reactive_gate_transition_matrix.csv">`results/notebook23_reactive_gate_transition_matrix.csv`</a>
- Forecast classification report CSV: <a href="results/notebook23_forecast_classification_report.csv">`results/notebook23_forecast_classification_report.csv`</a>

- Figure: <a href="figures/notebook23_decompression_risk_timeline.png">`figures/notebook23_decompression_risk_timeline.png`</a>
- Figure: <a href="figures/notebook23_reactive_vs_predictive_gate_timeline.png">`figures/notebook23_reactive_vs_predictive_gate_timeline.png`</a>
- Figure: <a href="figures/notebook23_reactive_vs_predictive_stability.png">`figures/notebook23_reactive_vs_predictive_stability.png`</a>
- Figure: <a href="figures/notebook23_reactive_vs_predictive_switch_rates.png">`figures/notebook23_reactive_vs_predictive_switch_rates.png`</a>
- Figure: <a href="figures/notebook23_action_counts.png">`figures/notebook23_action_counts.png`</a>
- Figure: <a href="figures/notebook23_stability_gain_by_macro_route.png">`figures/notebook23_stability_gain_by_macro_route.png`</a>
- Figure: <a href="figures/notebook23_predictive_gate_transition_matrix.png">`figures/notebook23_predictive_gate_transition_matrix.png`</a>
- Figure: <a href="figures/notebook23_pressure_risk_intervention.png">`figures/notebook23_pressure_risk_intervention.png`</a>
- Figure: <a href="figures/notebook23_avoided_fallback_summary.png">`figures/notebook23_avoided_fallback_summary.png`</a>

## Summary

|   windows |   roc_auc |   forecast_positive_windows |   actual_future_decompression_windows |   predictive_reroute_windows |   reactive_fallback_windows |   predictive_fallback_windows |   avoidable_fallback_windows |   mean_reactive_stability |   mean_predictive_stability |   mean_stability_gain |   mean_reactive_switch_rate |   mean_predictive_switch_rate |
|----------:|----------:|----------------------------:|--------------------------------------:|-----------------------------:|----------------------------:|------------------------------:|-----------------------------:|--------------------------:|----------------------------:|----------------------:|----------------------------:|------------------------------:|
|       240 |  0.955189 |                          21 |                                    28 |                           15 |                          11 |                             6 |                           19 |                  0.525255 |                    0.553589 |             0.0283333 |                    0.186263 |                      0.188826 |

## Predictive action summary

| predictive_action   |   windows |   mean_probability |   mean_pressure |   mean_cgcs |   mean_stability_gain |   avoidable_fallback_rate |
|:--------------------|----------:|-------------------:|----------------:|------------:|----------------------:|--------------------------:|
| monitor             |       193 |          0.0742822 |        0.366176 |    0.525796 |            0.00735751 |                  0        |
| retain_compressed   |        26 |          0.0346947 |        0.365855 |    0.758215 |            0.06       |                  0        |
| predictive_reroute  |        15 |          0.582068  |        0.640679 |    0.500804 |            0.216      |                  0.866667 |
| protective_fallback |         6 |          0.751807  |        0.760037 |    0.407737 |            0.0966667  |                  1        |

## Macro route summary

| macro_route   |   windows |   mean_probability |   mean_pressure |   mean_cgcs |   mean_stability_gain |   predictive_reroute_rate |   protective_fallback_rate |
|:--------------|----------:|-------------------:|----------------:|------------:|----------------------:|--------------------------:|---------------------------:|
| macro_1       |        26 |          0.174996  |        0.437776 |    0.537271 |             0.0530769 |                 0.0769231 |                  0.0769231 |
| macro_4       |        47 |          0.159265  |        0.38458  |    0.535345 |             0.0278723 |                 0.0851064 |                  0.0638298 |
| macro_3       |        39 |          0.117493  |        0.418102 |    0.554013 |             0.0374359 |                 0.0769231 |                  0         |
| macro_2       |        49 |          0.100203  |        0.369517 |    0.533377 |             0.0189796 |                 0.0204082 |                  0.0204082 |
| macro_0       |        79 |          0.0880109 |        0.385884 |    0.560487 |             0.0217722 |                 0.0632911 |                  0         |

## Feature importance

| feature             |   importance |
|:--------------------|-------------:|
| rolling_pressure    |    0.224015  |
| rolling_stability   |    0.221592  |
| rolling_residual    |    0.200987  |
| rolling_switch_rate |    0.118482  |
| macro_cgcs_score    |    0.114278  |
| rolling_volatility  |    0.0835203 |
| macro_route_id      |    0.0371263 |

## Predictive gate transition probabilities

|          |   accepted |    watch |   reroute |   fallback |
|:---------|-----------:|---------:|----------:|-----------:|
| accepted |  0.461538  | 0.538462 |  0        | 0          |
| watch    |  0.0677083 | 0.911458 |  0.015625 | 0.00520833 |
| reroute  |  0.0666667 | 0.2      |  0.533333 | 0.2        |
| fallback |  0         | 0        |  0.666667 | 0.333333   |

## Reactive gate transition probabilities

|          |   accepted |    watch |   fallback |
|:---------|-----------:|---------:|-----------:|
| accepted |  0.444444  | 0.518519 |  0.037037  |
| watch    |  0.0646766 | 0.910448 |  0.0248756 |
| fallback |  0.181818  | 0.454545 |  0.363636  |

## Forecast classification report

|              |   precision |   recall |   f1-score |    support |
|:-------------|------------:|---------:|-----------:|-----------:|
| 0            |    0.958904 | 0.990566 |   0.974478 | 212        |
| 1            |    0.904762 | 0.678571 |   0.77551  |  28        |
| accuracy     |    0.954167 | 0.954167 |   0.954167 |   0.954167 |
| macro avg    |    0.931833 | 0.834569 |   0.874994 | 240        |
| weighted avg |    0.952588 | 0.954167 |   0.951265 | 240        |

## Interpretation

- Predictive routing converts high decompression probability into route actions before fallback is observed.
- Predictive reroute windows act as intermediate states between watch and fallback.
- Protective fallback is reserved for high-risk, high-pressure windows.
- Stability gain estimates whether forecast-informed routing improves compressed-route reliability.
- Avoidable fallback windows identify where forecasting can reduce reactive collapse handling.

## Next step

Notebook 24 can build route-memory policy evaluation:
- compare routing policies across repeated trials,
- estimate stability distributions,
- score policy regret,
- select routing policies under CGCS-style constraints.

## Notebook 24

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/24_route_memory_policy_evaluation.ipynb)

# Report 24 — Route-Memory Policy Evaluation

This report compares route-memory policies across repeated trials.

Constraint view:
> route-memory policies should be compared by stability, fallback reduction, regret, and constraint alignment.

## Generated outputs

- Policy evaluation CSV: <a href="results/notebook24_route_memory_policy_evaluation.csv">`results/notebook24_route_memory_policy_evaluation.csv`</a>
- Policy evaluation JSON: <a href="results/notebook24_route_memory_policy_evaluation.json">`results/notebook24_route_memory_policy_evaluation.json`</a>
- Policy summary CSV: <a href="results/notebook24_policy_summary.csv">`results/notebook24_policy_summary.csv`</a>
- Trial summary CSV: <a href="results/notebook24_trial_summary.csv">`results/notebook24_trial_summary.csv`</a>
- Route-policy summary CSV: <a href="results/notebook24_route_policy_summary.csv">`results/notebook24_route_policy_summary.csv`</a>
- aggressive_predictive gate transition matrix CSV: <a href="results/notebook24_aggressive_predictive_gate_transition_matrix.csv">`results/notebook24_aggressive_predictive_gate_transition_matrix.csv`</a>
- cgcs_balanced gate transition matrix CSV: <a href="results/notebook24_cgcs_balanced_gate_transition_matrix.csv">`results/notebook24_cgcs_balanced_gate_transition_matrix.csv`</a>
- conservative_predictive gate transition matrix CSV: <a href="results/notebook24_conservative_predictive_gate_transition_matrix.csv">`results/notebook24_conservative_predictive_gate_transition_matrix.csv`</a>
- predictive gate transition matrix CSV: <a href="results/notebook24_predictive_gate_transition_matrix.csv">`results/notebook24_predictive_gate_transition_matrix.csv`</a>
- reactive gate transition matrix CSV: <a href="results/notebook24_reactive_gate_transition_matrix.csv">`results/notebook24_reactive_gate_transition_matrix.csv`</a>
- Figure: <a href="figures/notebook24_policy_constraint_score.png">`figures/notebook24_policy_constraint_score.png`</a>
- Figure: <a href="figures/notebook24_policy_regret.png">`figures/notebook24_policy_regret.png`</a>
- Figure: <a href="figures/notebook24_fallback_vs_reroute_rates.png">`figures/notebook24_fallback_vs_reroute_rates.png`</a>
- Figure: <a href="figures/notebook24_stability_distribution.png">`figures/notebook24_stability_distribution.png`</a>
- Figure: <a href="figures/notebook24_switch_rate_distribution.png">`figures/notebook24_switch_rate_distribution.png`</a>
- Figure: <a href="figures/notebook24_route_policy_constraint_heatmap.png">`figures/notebook24_route_policy_constraint_heatmap.png`</a>
- Figure: <a href="figures/notebook24_trial_constraint_score_timeline.png">`figures/notebook24_trial_constraint_score_timeline.png`</a>
- Figure: <a href="figures/notebook24_predictive_gate_transition_matrix.png">`figures/notebook24_predictive_gate_transition_matrix.png`</a>
- Figure: <a href="figures/notebook24_policy_recommendation_summary.png">`figures/notebook24_policy_recommendation_summary.png`</a>

## Summary

| policy                  |   windows |   mean_stability |   std_stability |   mean_constraint_score |   mean_cost |   mean_regret |   fallback_rate |   reroute_rate |   watch_rate |   accepted_rate |   mean_switch_rate |
|:------------------------|----------:|-----------------:|----------------:|------------------------:|------------:|--------------:|----------------:|---------------:|-------------:|----------------:|-------------------:|
| cgcs_balanced           |      9600 |         0.588481 |        0.120581 |                0.626514 |    0.208521 |     0.106736  |       0.0619792 |       0.424375 |     0.399792 |        0.113854 |           0.458154 |
| predictive              |      9600 |         0.595671 |        0.131342 |                0.624655 |    0.238448 |     0.136662  |       0.0935417 |       0.448854 |     0.346146 |        0.111458 |           0.465712 |
| conservative_predictive |      9600 |         0.564233 |        0.114745 |                0.622721 |    0.13814  |     0.036354  |       0.0226042 |       0.193438 |     0.683438 |        0.100521 |           0.364311 |
| aggressive_predictive   |      9600 |         0.592404 |        0.154928 |                0.61063  |    0.352543 |     0.250757  |       0.178333  |       0.629062 |     0.114479 |        0.078125 |           0.432945 |
| reactive                |      9600 |         0.509743 |        0.180596 |                0.584941 |    0.20083  |     0.0990446 |       0.129167  |       0        |     0.733333 |        0.1375   |           0.259097 |

## Trial summary preview

|   trial | policy                  |   mean_stability |   mean_constraint_score |   mean_cost |   mean_regret |   fallback_rate |   reroute_rate |   mean_switch_rate |
|--------:|:------------------------|-----------------:|------------------------:|------------:|--------------:|----------------:|---------------:|-------------------:|
|       0 | aggressive_predictive   |         0.598535 |                0.617131 |    0.33552  |     0.23425   |       0.154167  |       0.658333 |           0.419501 |
|       0 | cgcs_balanced           |         0.589077 |                0.628348 |    0.200233 |     0.0989625 |       0.0541667 |       0.420833 |           0.456097 |
|       0 | conservative_predictive |         0.563327 |                0.623673 |    0.128791 |     0.0275208 |       0.0166667 |       0.175    |           0.325069 |
|       0 | predictive              |         0.59266  |                0.622906 |    0.240408 |     0.139137  |       0.1       |       0.429167 |           0.474152 |
|       0 | reactive                |         0.509743 |                0.585365 |    0.200491 |     0.0992208 |       0.129167  |       0        |           0.259097 |
|       1 | aggressive_predictive   |         0.592202 |                0.611037 |    0.344103 |     0.244246  |       0.170833  |       0.616667 |           0.461446 |
|       1 | cgcs_balanced           |         0.588827 |                0.627812 |    0.197407 |     0.09755   |       0.05      |       0.416667 |           0.462096 |
|       1 | conservative_predictive |         0.564202 |                0.622337 |    0.136878 |     0.0370208 |       0.0208333 |       0.191667 |           0.385082 |
|       1 | predictive              |         0.597743 |                0.626378 |    0.232023 |     0.132167  |       0.0833333 |       0.458333 |           0.455264 |
|       1 | reactive                |         0.509743 |                0.584303 |    0.20134  |     0.101483  |       0.129167  |       0        |           0.259097 |
|       2 | aggressive_predictive   |         0.589952 |                0.608006 |    0.360732 |     0.259071  |       0.1875    |       0.620833 |           0.436446 |
|       2 | cgcs_balanced           |         0.586327 |                0.624681 |    0.210361 |     0.1087    |       0.0666667 |       0.408333 |           0.454936 |
|       2 | conservative_predictive |         0.566702 |                0.624331 |    0.135832 |     0.0341708 |       0.0166667 |       0.2125   |           0.358478 |
|       2 | predictive              |         0.596493 |                0.626248 |    0.228978 |     0.127317  |       0.0833333 |       0.445833 |           0.471097 |
|       2 | reactive                |         0.509743 |                0.584673 |    0.201044 |     0.0993833 |       0.129167  |       0        |           0.259097 |

## Route-policy summary

| macro_route   | policy                  |   windows |   mean_stability |   mean_constraint_score |   mean_regret |   fallback_rate |   reroute_rate |
|:--------------|:------------------------|----------:|-----------------:|------------------------:|--------------:|----------------:|---------------:|
| macro_0       | aggressive_predictive   |      3440 |         0.570018 |                0.582417 |     0.298358  |       0.260465  |       0.578779 |
| macro_0       | cgcs_balanced           |      3440 |         0.577943 |                0.609787 |     0.114964  |       0.0991279 |       0.405814 |
| macro_0       | conservative_predictive |      3440 |         0.563024 |                0.612497 |     0.0405875 |       0.0412791 |       0.25     |
| macro_0       | predictive              |      3440 |         0.579039 |                0.603859 |     0.148879  |       0.14157   |       0.395349 |
| macro_0       | reactive                |      3440 |         0.496361 |                0.565861 |     0.116996  |       0.174419  |       0        |
| macro_1       | aggressive_predictive   |      1320 |         0.624931 |                0.635767 |     0.237996  |       0.141667  |       0.623485 |
| macro_1       | cgcs_balanced           |      1320 |         0.610121 |                0.644729 |     0.108045  |       0.0424242 |       0.361364 |
| macro_1       | conservative_predictive |      1320 |         0.582727 |                0.637749 |     0.0564909 |       0.0159091 |       0.12803  |
| macro_1       | predictive              |      1320 |         0.618052 |                0.643243 |     0.140401  |       0.0734848 |       0.400758 |
| macro_1       | reactive                |      1320 |         0.562333 |                0.627432 |     0.0356015 |       0.030303  |       0        |
| macro_2       | aggressive_predictive   |      1720 |         0.620628 |                0.637785 |     0.215635  |       0.101163  |       0.701163 |
| macro_2       | cgcs_balanced           |      1720 |         0.610523 |                0.642464 |     0.128783  |       0.0430233 |       0.516279 |
| macro_2       | conservative_predictive |      1720 |         0.573232 |                0.631472 |     0.0395047 |       0.0168605 |       0.137791 |
| macro_2       | predictive              |      1720 |         0.627337 |                0.648492 |     0.141674  |       0.0476744 |       0.581395 |
| macro_2       | reactive                |      1720 |         0.533977 |                0.604345 |     0.0849733 |       0.0930233 |       0        |
| macro_3       | aggressive_predictive   |      1720 |         0.590334 |                0.61764  |     0.22355   |       0.122674  |       0.711628 |
| macro_3       | cgcs_balanced           |      1720 |         0.57791  |                0.628194 |     0.0857076 |       0.019186  |       0.461047 |
| macro_3       | conservative_predictive |      1720 |         0.547694 |                0.618986 |     0.0300919 |       0         |       0.201744 |
| macro_3       | predictive              |      1720 |         0.590003 |                0.627886 |     0.122632  |       0.0534884 |       0.518023 |
| macro_3       | reactive                |      1720 |         0.491398 |                0.579025 |     0.101981  |       0.116279  |       0        |
| macro_4       | aggressive_predictive   |      1400 |         0.584607 |                0.614277 |     0.2224    |       0.174286  |       0.567857 |
| macro_4       | cgcs_balanced           |      1400 |         0.579879 |                0.628779 |     0.0840329 |       0.065     |       0.371429 |
| macro_4       | conservative_predictive |      1400 |         0.559029 |                0.62751  |     0.0107879 |       0.0178571 |       0.174286 |
| macro_4       | predictive              |      1400 |         0.583493 |                0.624974 |     0.114199  |       0.1       |       0.377857 |
| macro_4       | reactive                |      1400 |         0.485807 |                0.575186 |     0.128433  |       0.171429  |       0        |

## Policy recommendation

| policy                  |   windows |   mean_stability |   std_stability |   mean_constraint_score |   mean_cost |   mean_regret |   fallback_rate |   reroute_rate |   watch_rate |   accepted_rate |   mean_switch_rate |   rank_score |
|:------------------------|----------:|-----------------:|----------------:|------------------------:|------------:|--------------:|----------------:|---------------:|-------------:|----------------:|-------------------:|-------------:|
| conservative_predictive |      9600 |         0.564233 |        0.114745 |                0.622721 |    0.13814  |     0.036354  |       0.0226042 |       0.193438 |     0.683438 |        0.100521 |           0.364311 |     0.707761 |
| cgcs_balanced           |      9600 |         0.588481 |        0.120581 |                0.626514 |    0.208521 |     0.106736  |       0.0619792 |       0.424375 |     0.399792 |        0.113854 |           0.458154 |     0.653298 |
| predictive              |      9600 |         0.595671 |        0.131342 |                0.624655 |    0.238448 |     0.136662  |       0.0935417 |       0.448854 |     0.346146 |        0.111458 |           0.465712 |     0.625749 |
| reactive                |      9600 |         0.509743 |        0.180596 |                0.584941 |    0.20083  |     0.0990446 |       0.129167  |       0        |     0.733333 |        0.1375   |           0.259097 |     0.613041 |
| aggressive_predictive   |      9600 |         0.592404 |        0.154928 |                0.61063  |    0.352543 |     0.250757  |       0.178333  |       0.629062 |     0.114479 |        0.078125 |           0.432945 |     0.515603 |

## Predictive gate transition probabilities

|          |   accepted |     watch |   reroute |   fallback |
|:---------|-----------:|----------:|----------:|-----------:|
| accepted |  0.518519  | 0.37037   |  0.111111 |  0         |
| watch    |  0.104651  | 0.569767  |  0.313953 |  0.0116279 |
| reroute  |  0.0392157 | 0.245098  |  0.558824 |  0.156863  |
| fallback |  0         | 0.0833333 |  0.666667 |  0.25      |

## Interpretation

- Reactive routing minimizes early intervention but waits until decompression is already observed.
- Predictive routing can reduce reactive fallback handling by inserting reroute states before collapse.
- Conservative predictive routing reduces unnecessary reroutes but may miss some avoidable fallback windows.
- Aggressive predictive routing increases reroute pressure and may raise switching cost.
- CGCS-balanced routing weighs stability, pressure, fallback avoidance, and route switching together.
- Best policy by recommendation score in this run: `conservative_predictive`.

## Next step

Notebook 25 can build policy-regret phase diagrams:
- vary forecast threshold,
- vary fallback penalty,
- vary reroute cost,
- map stable regions of route-memory policy choice.

##Notebook 25

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/25_constraint_budget_allocation.ipynb)
# Report 25 — Constraint Budget Allocation

This report adds finite resource allocation to predictive route-memory policy evaluation.

Constraint view:
> predictive routing is useful only when finite constraint budgets are allocated before fallback pressure dominates.

## Generated outputs
)
- Budget allocation CSV: `results/notebook25_constraint_budget_allocation.csv` (49MB)
- Budget allocation JSON: `results/notebook25_constraint_budget_allocation.json` (140MB)
- Budget summary CSV: <a href="results/notebook25_budget_summary.csv">`results/notebook25_budget_summary.csv`</a>
- Policy-budget summary CSV: <a href="results/notebook25_policy_budget_summary.csv">`results/notebook25_policy_budget_summary.csv`</a>
- Route-budget summary CSV: <a href="results/notebook25_route_budget_summary.csv">`results/notebook25_route_budget_summary.csv`</a>
- Figure: <a href="figures/notebook25_budget_recommendation_score.png">`figures/notebook25_budget_recommendation_score.png`</a>
- Figure: <a href="figures/notebook25_adjusted_constraint_score.png">`figures/notebook25_adjusted_constraint_score.png`</a>
- Figure: <a href="figures/notebook25_budget_regret.png">`figures/notebook25_budget_regret.png`</a>
- Figure: <a href="figures/notebook25_allocation_rates.png">`figures/notebook25_allocation_rates.png`</a>
- Figure: <a href="figures/notebook25_avoided_fallback_rate.png">`figures/notebook25_avoided_fallback_rate.png`</a>
- Figure: <a href="figures/notebook25_policy_budget_constraint_heatmap.png">`figures/notebook25_policy_budget_constraint_heatmap.png`</a>
- Figure: <a href="figures/notebook25_route_budget_constraint_heatmap.png">`figures/notebook25_route_budget_constraint_heatmap.png`</a>
- Figure: <a href="figures/notebook25_budget_priority_timeline.png">`figures/notebook25_budget_priority_timeline.png`</a>
- Figure: <a href="figures/notebook25_adjusted_cost_timeline.png">`figures/notebook25_adjusted_cost_timeline.png`</a>
- Figure: <a href="figures/notebook25_budget_frontier.png">`figures/notebook25_budget_frontier.png`</a>

## Budget strategy summary

| budget_strategy          |   windows |   mean_adjusted_constraint_score |   mean_adjusted_stability |   mean_adjusted_cost |   mean_budget_regret |   reroute_allocation_rate |   decompression_allocation_rate |   monitor_allocation_rate |   avoided_fallback_rate |   mean_budget_switch_rate |
|:-------------------------|----------:|---------------------------------:|--------------------------:|---------------------:|---------------------:|--------------------------:|--------------------------------:|--------------------------:|------------------------:|--------------------------:|
| cgcs_balanced_budget     |     36000 |                         0.576925 |                  0.588119 |             0.127534 |            0.0845606 |                  0.265694 |                        0.185528 |                  0.489833 |               0.0146389 |                  0.346347 |
| uniform_budget           |     36000 |                         0.575909 |                  0.586942 |             0.130251 |            0.0856052 |                  0.2515   |                        0.176583 |                  0.499833 |               0.0146389 |                  0.346347 |
| pressure_first           |     36000 |                         0.574512 |                  0.587725 |             0.126936 |            0.0830322 |                  0.277889 |                        0.185528 |                  0.452306 |               0.0146389 |                  0.346347 |
| forecast_first           |     36000 |                         0.573683 |                  0.588477 |             0.12437  |            0.0839174 |                  0.296278 |                        0.185917 |                  0.439667 |               0.0146389 |                  0.346347 |
| regret_minimizing_budget |     36000 |                         0.572114 |                  0.585748 |             0.131856 |            0.0857567 |                  0.239222 |                        0.182917 |                  0.469833 |               0.0146389 |                  0.346347 |

## Policy-budget summary

| policy                  | budget_strategy          |   windows |   mean_adjusted_constraint_score |   mean_adjusted_stability |   mean_adjusted_cost |   mean_budget_regret |   avoided_fallback_rate |   reroute_allocation_rate |   decompression_allocation_rate |
|:------------------------|:-------------------------|----------:|---------------------------------:|--------------------------:|---------------------:|---------------------:|------------------------:|--------------------------:|--------------------------------:|
| aggressive_predictive   | cgcs_balanced_budget     |      7200 |                         0.586425 |                  0.630229 |            0.17849   |            0.135516  |              0          |                 0.320833  |                        0.190556 |
| aggressive_predictive   | forecast_first           |      7200 |                         0.590898 |                  0.634027 |            0.169208  |            0.128755  |              0          |                 0.383333  |                        0.19125  |
| aggressive_predictive   | pressure_first           |      7200 |                         0.589321 |                  0.631479 |            0.177575  |            0.133671  |              0          |                 0.341667  |                        0.190556 |
| aggressive_predictive   | regret_minimizing_budget |      7200 |                         0.583316 |                  0.627765 |            0.184379  |            0.138279  |              0          |                 0.283333  |                        0.1875   |
| aggressive_predictive   | uniform_budget           |      7200 |                         0.582903 |                  0.628142 |            0.182935  |            0.138289  |              0          |                 0.3       |                        0.178611 |
| cgcs_balanced           | cgcs_balanced_budget     |      7200 |                         0.587843 |                  0.586291 |            0.104252  |            0.0612784 |              0.00777778 |                 0.314306  |                        0.18375  |
| cgcs_balanced           | forecast_first           |      7200 |                         0.580508 |                  0.584837 |            0.103544  |            0.0630914 |              0.00777778 |                 0.33125   |                        0.184167 |
| cgcs_balanced           | pressure_first           |      7200 |                         0.582286 |                  0.585116 |            0.103855  |            0.0599517 |              0.00777778 |                 0.325972  |                        0.18375  |
| cgcs_balanced           | regret_minimizing_budget |      7200 |                         0.580399 |                  0.583237 |            0.109471  |            0.0633716 |              0.00777778 |                 0.2825    |                        0.181667 |
| cgcs_balanced           | uniform_budget           |      7200 |                         0.587378 |                  0.585136 |            0.106967  |            0.062321  |              0.00777778 |                 0.297083  |                        0.174861 |
| conservative_predictive | cgcs_balanced_budget     |      7200 |                         0.589896 |                  0.586655 |            0.0964116 |            0.0534381 |              0          |                 0.307083  |                        0.187083 |
| conservative_predictive | forecast_first           |      7200 |                         0.581668 |                  0.584852 |            0.0966193 |            0.0561666 |              0          |                 0.318056  |                        0.187639 |
| conservative_predictive | pressure_first           |      7200 |                         0.583661 |                  0.585238 |            0.0966737 |            0.0527701 |              0          |                 0.314722  |                        0.187083 |
| conservative_predictive | regret_minimizing_budget |      7200 |                         0.583544 |                  0.583898 |            0.100642  |            0.0545419 |              0          |                 0.281528  |                        0.183889 |
| conservative_predictive | uniform_budget           |      7200 |                         0.590098 |                  0.585808 |            0.0987731 |            0.0541274 |              0          |                 0.295     |                        0.178194 |
| predictive              | cgcs_balanced_budget     |      7200 |                         0.594539 |                  0.61095  |            0.137146  |            0.0941725 |              0          |                 0.320833  |                        0.18125  |
| predictive              | forecast_first           |      7200 |                         0.599409 |                  0.614675 |            0.129344  |            0.0888917 |              0          |                 0.383333  |                        0.18125  |
| predictive              | pressure_first           |      7200 |                         0.59703  |                  0.612196 |            0.13424   |            0.0903366 |              0          |                 0.341667  |                        0.18125  |
| predictive              | regret_minimizing_budget |      7200 |                         0.591464 |                  0.608593 |            0.142521  |            0.0964217 |              0          |                 0.283333  |                        0.179722 |
| predictive              | uniform_budget           |      7200 |                         0.591917 |                  0.609321 |            0.140858  |            0.0962126 |              0          |                 0.3       |                        0.175833 |
| reactive                | cgcs_balanced_budget     |      7200 |                         0.525923 |                  0.526472 |            0.121371  |            0.0783976 |              0.0654167  |                 0.0654167 |                        0.185    |
| reactive                | forecast_first           |      7200 |                         0.515932 |                  0.523991 |            0.123135  |            0.0826821 |              0.0654167  |                 0.0654167 |                        0.185278 |
| reactive                | pressure_first           |      7200 |                         0.52026  |                  0.524597 |            0.122335  |            0.0784313 |              0.0654167  |                 0.0654167 |                        0.185    |
| reactive                | regret_minimizing_budget |      7200 |                         0.521844 |                  0.525248 |            0.122268  |            0.0761688 |              0.0654167  |                 0.0654167 |                        0.181806 |
| reactive                | uniform_budget           |      7200 |                         0.527249 |                  0.526301 |            0.121722  |            0.077076  |              0.0654167  |                 0.0654167 |                        0.175417 |

## Route-budget summary

| macro_route   | budget_strategy          |   windows |   mean_adjusted_constraint_score |   mean_adjusted_stability |   mean_budget_regret |   avoided_fallback_rate |   allocation_rate |
|:--------------|:-------------------------|----------:|---------------------------------:|--------------------------:|---------------------:|------------------------:|------------------:|
| macro_0       | cgcs_balanced_budget     |      7271 |                         0.577109 |                  0.588511 |            0.0855027 |               0.0178792 |          0.75973  |
| macro_0       | forecast_first           |      7271 |                         0.574664 |                  0.589222 |            0.0846836 |               0.0178792 |          0.732086 |
| macro_0       | pressure_first           |      7271 |                         0.574789 |                  0.588094 |            0.0841206 |               0.0178792 |          0.738413 |
| macro_0       | regret_minimizing_budget |      7271 |                         0.572718 |                  0.586273 |            0.0864827 |               0.0178792 |          0.725897 |
| macro_0       | uniform_budget           |      7271 |                         0.576345 |                  0.587365 |            0.0864081 |               0.0178792 |          0.757255 |
| macro_1       | cgcs_balanced_budget     |      7176 |                         0.57583  |                  0.587622 |            0.0866324 |               0.0163043 |          0.762263 |
| macro_1       | forecast_first           |      7176 |                         0.572298 |                  0.587826 |            0.0861992 |               0.0163043 |          0.726031 |
| macro_1       | pressure_first           |      7176 |                         0.573533 |                  0.587317 |            0.0849174 |               0.0163043 |          0.740663 |
| macro_1       | regret_minimizing_budget |      7176 |                         0.571172 |                  0.585321 |            0.0878166 |               0.0163043 |          0.726031 |
| macro_1       | uniform_budget           |      7176 |                         0.575012 |                  0.586517 |            0.0875239 |               0.0163043 |          0.759058 |
| macro_2       | cgcs_balanced_budget     |      7237 |                         0.577913 |                  0.589712 |            0.0837569 |               0.0142324 |          0.762747 |
| macro_2       | forecast_first           |      7237 |                         0.575061 |                  0.59022  |            0.0828094 |               0.0142324 |          0.731933 |
| macro_2       | pressure_first           |      7237 |                         0.575765 |                  0.589422 |            0.0820921 |               0.0142324 |          0.742573 |
| macro_2       | regret_minimizing_budget |      7237 |                         0.572759 |                  0.587164 |            0.085304  |               0.0142324 |          0.722675 |
| macro_2       | uniform_budget           |      7237 |                         0.57678  |                  0.588482 |            0.0847986 |               0.0142324 |          0.756943 |
| macro_3       | cgcs_balanced_budget     |      7162 |                         0.577203 |                  0.588128 |            0.0827414 |               0.0118682 |          0.763334 |
| macro_3       | forecast_first           |      7162 |                         0.573236 |                  0.588248 |            0.0821546 |               0.0118682 |          0.723122 |
| macro_3       | pressure_first           |      7162 |                         0.574608 |                  0.587667 |            0.0813149 |               0.0118682 |          0.739458 |
| macro_3       | regret_minimizing_budget |      7162 |                         0.571908 |                  0.585639 |            0.0843132 |               0.0118682 |          0.721586 |
| macro_3       | uniform_budget           |      7162 |                         0.575972 |                  0.586881 |            0.0840675 |               0.0118682 |          0.756632 |
| macro_4       | cgcs_balanced_budget     |      7154 |                         0.576559 |                  0.5866   |            0.0841592 |               0.0128599 |          0.755801 |
| macro_4       | forecast_first           |      7154 |                         0.573129 |                  0.586836 |            0.0837354 |               0.0128599 |          0.720436 |
| macro_4       | pressure_first           |      7154 |                         0.573847 |                  0.586101 |            0.0827055 |               0.0128599 |          0.73092  |
| macro_4       | regret_minimizing_budget |      7154 |                         0.571997 |                  0.58432  |            0.0848555 |               0.0128599 |          0.720436 |
| macro_4       | uniform_budget           |      7154 |                         0.575422 |                  0.585438 |            0.08522   |               0.0128599 |          0.749651 |

## Recommendation summary

| budget_strategy          |   windows |   mean_adjusted_constraint_score |   mean_adjusted_stability |   mean_adjusted_cost |   mean_budget_regret |   reroute_allocation_rate |   decompression_allocation_rate |   monitor_allocation_rate |   avoided_fallback_rate |   mean_budget_switch_rate |   recommendation_score |
|:-------------------------|----------:|---------------------------------:|--------------------------:|---------------------:|---------------------:|--------------------------:|--------------------------------:|--------------------------:|------------------------:|--------------------------:|-----------------------:|
| pressure_first           |     36000 |                         0.574512 |                  0.587725 |             0.126936 |            0.0830322 |                  0.277889 |                        0.185528 |                  0.452306 |               0.0146389 |                  0.346347 |               0.357292 |
| forecast_first           |     36000 |                         0.573683 |                  0.588477 |             0.12437  |            0.0839174 |                  0.296278 |                        0.185917 |                  0.439667 |               0.0146389 |                  0.346347 |               0.355126 |
| cgcs_balanced_budget     |     36000 |                         0.576925 |                  0.588119 |             0.127534 |            0.0845606 |                  0.265694 |                        0.185528 |                  0.489833 |               0.0146389 |                  0.346347 |               0.354671 |
| uniform_budget           |     36000 |                         0.575909 |                  0.586942 |             0.130251 |            0.0856052 |                  0.2515   |                        0.176583 |                  0.499833 |               0.0146389 |                  0.346347 |               0.351585 |
| regret_minimizing_budget |     36000 |                         0.572114 |                  0.585748 |             0.131856 |            0.0857567 |                  0.239222 |                        0.182917 |                  0.469833 |               0.0146389 |                  0.346347 |               0.349605 |

## Interpretation

- Budget allocation turns predictive decompression into a finite-resource routing problem.
- Pressure-first allocation prioritizes high-pressure windows, but can overspend on noisy pressure spikes.
- Forecast-first allocation prioritizes expected decompression, but can miss low-probability high-cost collapse.
- CGCS-balanced allocation spreads resources across pressure, forecast risk, regret, and constraint score.
- Regret-minimizing allocation learns from policy cost but may under-allocate to early weak signals.
- Best budget strategy by recommendation score in this run: `pressure_first`.

## Next step

Notebook 26 can build budget phase diagrams:
- sweep reroute budget,
- sweep decompression budget,
- vary fallback penalty,
- identify stable budget regimes and failure boundaries.

## Notebook 26

# Next

| Notebook | Direction                                       |
| -------- | ----------------------------------------------- |
| 17       | hierarchical prototype trees                    |
| 18       | prototype merge/split events                    |
| 19       | online Bayesian regime confidence               |
| 20       | CGCS stability scoring over prototype evolution |
| 21       | manifold projection of prototype trajectories   |
| 22       | drift-wave propagation analysis                 |
| 23       | adaptive alarm thresholds                       |
| 24       | prototype genealogy graph                       |
| 25       | online intervention simulation                  |
| 00       | ZOS left5 GOS                                   |

