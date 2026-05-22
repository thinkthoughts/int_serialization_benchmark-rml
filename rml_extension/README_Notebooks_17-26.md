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

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/thinkthoughts/int_serialization_benchmark-rml/blob/main/rml_extension/notebooks/26_regime_shift_constraint_routing.ipynb)

# Report 26 — Regime-Shift Constraint Routing

Notebook 26 evaluates route-memory policies under nonstationary operating regimes.

Constraint view:
> adaptive routing is useful only when policies remain coherent across changing regimes, not only during stable plateaus.

## Generated outputs

- Regime-shift routing CSV: <a href="results/notebook26_regime_shift_constraint_routing.csv">`results/notebook26_regime_shift_constraint_routing.csv`</a>
- Regime-shift routing JSON: <a href="results/notebook26_regime_shift_constraint_routing.json">`results/notebook26_regime_shift_constraint_routing.json`</a>
- Policy summary CSV: <a href="results/notebook26_policy_summary.csv">`results/notebook26_policy_summary.csv`</a>
- Regime summary CSV: <a href="results/notebook26_regime_summary.csv">`results/notebook26_regime_summary.csv`</a>
- Regime transition matrix CSV: <a href="results/notebook26_regime_transition_matrix.csv">`results/notebook26_regime_transition_matrix.csv`</a>
- Route persistence CSV: <a href="results/notebook26_route_persistence.csv">`results/notebook26_route_persistence.csv`</a>
- Figure: <a href="figures/notebook26_regime_timeline.png">`figures/notebook26_regime_timeline.png`</a>
- Figure: <a href="figures/notebook26_constraint_score_by_regime.png">`figures/notebook26_constraint_score_by_regime.png`</a>
- Figure: <a href="figures/notebook26_regret_by_regime.png">`figures/notebook26_regret_by_regime.png`</a>
- Figure: <a href="figures/notebook26_recovery_curve.png">`figures/notebook26_recovery_curve.png`</a>
- Figure: <a href="figures/notebook26_policy_frontier.png">`figures/notebook26_policy_frontier.png`</a>
- Figure: <a href="figures/notebook26_regime_transition_matrix.png">`figures/notebook26_regime_transition_matrix.png`</a>
- Figure: <a href="figures/notebook26_memory_quality_timeline.png">`figures/notebook26_memory_quality_timeline.png`</a>
- Figure: <a href="figures/notebook26_route_persistence.png">`figures/notebook26_route_persistence.png`</a>
- Figure: <a href="figures/notebook26_recommendation_score.png">`figures/notebook26_recommendation_score.png`</a>
- Figure: <a href="figures/notebook26_route_volatility_timeline.png">`figures/notebook26_route_volatility_timeline.png`</a>

## Summary

|   windows |   regimes |   policies |   mean_constraint_score |   mean_regret |   mean_recovery_score |   mean_route_persistence |   mean_recommendation_score |
|----------:|----------:|-----------:|------------------------:|--------------:|----------------------:|-------------------------:|----------------------------:|
|       240 |         5 |          5 |                0.534343 |      0.198753 |              0.373813 |                 0.436976 |                    0.332934 |

## Policy summary

| policy                  |   windows |   mean_constraint_score |   mean_regret |   mean_recovery_score |   mean_route_persistence |   mean_route_volatility |   mean_memory_quality |   mean_recommendation_score |
|:------------------------|----------:|------------------------:|--------------:|----------------------:|-------------------------:|------------------------:|----------------------:|----------------------------:|
| cgcs_balanced           |       240 |                0.594981 |      0.134466 |              0.534883 |                 0.527465 |                0.538605 |              0.440466 |                    0.422367 |
| conservative_predictive |       240 |                0.572732 |      0.168349 |              0.448604 |                 0.516186 |                0.54778  |              0.426445 |                    0.385898 |
| predictive              |       240 |                0.537906 |      0.198142 |              0.382767 |                 0.398616 |                0.663787 |              0.416017 |                    0.328708 |
| aggressive_predictive   |       240 |                0.505873 |      0.210474 |              0.283868 |                 0.314878 |                0.745362 |              0.401599 |                    0.275564 |
| reactive                |       240 |                0.460223 |      0.282335 |              0.218942 |                 0.427736 |                0.630841 |              0.390512 |                    0.252132 |

## Regime summary

| regime        | policy                  |   windows |   mean_constraint_score |   mean_regret |   mean_recovery_score |   mean_route_persistence |   mean_route_volatility |   mean_decompression_need |   mean_recommendation_score |
|:--------------|:------------------------|----------:|------------------------:|--------------:|----------------------:|-------------------------:|------------------------:|--------------------------:|----------------------------:|
| adversarial   | aggressive_predictive   |        48 |                0.383244 |     0.270885  |              0.219183 |                 0.151232 |                0.93057  |                  0.558445 |                    0.169923 |
| adversarial   | cgcs_balanced           |        48 |                0.555763 |     0.196324  |              0.384655 |                 0.352073 |                0.735382 |                  0.462032 |                    0.326964 |
| adversarial   | conservative_predictive |        48 |                0.553248 |     0.232089  |              0.328054 |                 0.340775 |                0.744655 |                  0.493917 |                    0.304143 |
| adversarial   | predictive              |        48 |                0.466951 |     0.261259  |              0.282655 |                 0.220029 |                0.863629 |                  0.525227 |                    0.231379 |
| adversarial   | reactive                |        48 |                0.405247 |     0.348376  |              0.177769 |                 0.251067 |                0.828531 |                  0.564788 |                    0.173641 |
| overload      | aggressive_predictive   |        48 |                0.56644  |     0.204608  |              0.245259 |                 0.304471 |                0.744061 |                  0.43991  |                    0.289177 |
| overload      | cgcs_balanced           |        48 |                0.599507 |     0.128201  |              0.483139 |                 0.528772 |                0.525111 |                  0.357943 |                    0.414217 |
| overload      | conservative_predictive |        48 |                0.556651 |     0.157886  |              0.399994 |                 0.51622  |                0.535741 |                  0.393732 |                    0.371193 |
| overload      | predictive              |        48 |                0.558023 |     0.187582  |              0.338362 |                 0.401329 |                0.648798 |                  0.405501 |                    0.329238 |
| overload      | reactive                |        48 |                0.423085 |     0.272663  |              0.183291 |                 0.429362 |                0.617436 |                  0.454525 |                    0.232436 |
| recovery      | aggressive_predictive   |        48 |                0.54803  |     0.154573  |              0.341853 |                 0.456192 |                0.586229 |                  0.285641 |                    0.343783 |
| recovery      | cgcs_balanced           |        48 |                0.651672 |     0.0767677 |              0.65925  |                 0.665399 |                0.384439 |                  0.210194 |                    0.510397 |
| recovery      | conservative_predictive |        48 |                0.625503 |     0.113985  |              0.550525 |                 0.654961 |                0.391943 |                  0.232096 |                    0.467002 |
| recovery      | predictive              |        48 |                0.590371 |     0.140609  |              0.468541 |                 0.542036 |                0.503597 |                  0.252177 |                    0.407705 |
| recovery      | reactive                |        48 |                0.521363 |     0.220149  |              0.260923 |                 0.567871 |                0.474159 |                  0.310726 |                    0.325065 |
| sparse_memory | aggressive_predictive   |        48 |                0.536412 |     0.227439  |              0.249373 |                 0.295166 |                0.781463 |                  0.547144 |                    0.272244 |
| sparse_memory | cgcs_balanced           |        48 |                0.602087 |     0.15302   |              0.45083  |                 0.503488 |                0.578451 |                  0.456812 |                    0.398069 |
| sparse_memory | conservative_predictive |        48 |                0.576857 |     0.183913  |              0.381475 |                 0.498186 |                0.581949 |                  0.490034 |                    0.365985 |
| sparse_memory | predictive              |        48 |                0.553166 |     0.217547  |              0.328911 |                 0.377347 |                0.701249 |                  0.509903 |                    0.314524 |
| sparse_memory | reactive                |        48 |                0.482766 |     0.300841  |              0.196811 |                 0.404045 |                0.670198 |                  0.557114 |                    0.24739  |
| stable        | aggressive_predictive   |        48 |                0.495241 |     0.194867  |              0.363673 |                 0.36733  |                0.684486 |                  0.386149 |                    0.302692 |
| stable        | cgcs_balanced           |        48 |                0.565878 |     0.118018  |              0.69654  |                 0.587593 |                0.469641 |                  0.305027 |                    0.462187 |
| stable        | conservative_predictive |        48 |                0.551399 |     0.153871  |              0.58297  |                 0.570791 |                0.484613 |                  0.331524 |                    0.421169 |
| stable        | predictive              |        48 |                0.521021 |     0.183717  |              0.495365 |                 0.452338 |                0.601662 |                  0.35318  |                    0.360693 |
| stable        | reactive                |        48 |                0.468653 |     0.269649  |              0.275916 |                 0.486337 |                0.563879 |                  0.406273 |                    0.282127 |

## Regime transition probabilities

|               |   stable |   overload |   sparse_memory |   adversarial |   recovery |
|:--------------|---------:|-----------:|----------------:|--------------:|-----------:|
| stable        | 0.979167 |  0.0208333 |       0         |     0         |  0         |
| overload      | 0        |  0.979167  |       0.0208333 |     0         |  0         |
| sparse_memory | 0        |  0         |       0.979167  |     0.0208333 |  0         |
| adversarial   | 0        |  0         |       0         |     0.979167  |  0.0208333 |
| recovery      | 0        |  0         |       0         |     0         |  1         |

## Interpretation

- Stable regimes reward route persistence and low decompression pressure.
- Overload regimes reward predictive and pressure-aware adaptation.
- Sparse-memory regimes expose whether a policy can recover from degraded retrieval quality.
- Adversarial regimes penalize aggressive rerouting and highlight robustness differences.
- Recovery regimes test whether policies can return to coherent compressed routing after instability.

## Next step

Notebook 27 can start a new group: distributed adaptive coordination across multiple routing agents.
