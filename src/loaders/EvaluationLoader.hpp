#ifndef EVALUATION_LOADER_HPP
#define EVALUATION_LOADER_HPP

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Entry representing an evaluation result for a TAP instance.
 *
 * Stores the objective value and a flag indicating whether this value
 * corresponds to a proven optimal solution (no '*' in the file).
 */
struct EvaluationEntry {
    double value;      ///< Objective value for the instance
    bool is_optimal;   ///< true if value is optimal; false if marked with '*'
};

/**
 * @brief Load optimal solution values from a semicolon-delimited file.
 *
 * Each non-header line should have the format:
 *     instance_name;value[*]
 * where a trailing '*' indicates the solution is not proven optimal.
 *
 * @param filename Path to the optimal values file.
 * @return Map from instance filename to its EvaluationEntry.
 */
std::unordered_map<std::string, EvaluationEntry>
load_opt_values(const std::string& filename);

/**
 * @brief Load baseline objective values from a semicolon-delimited file.
 *
 * Each non-header line should have the format:
 *     instance_name;value
 *
 * @param filename Path to the baseline values file.
 * @return Map from instance filename to its baseline objective value.
 */
std::unordered_map<std::string, double>
load_baseline_values(const std::string& filename);

/**
 * @brief Print a formatted comparison table of greedy, baseline, and optimal values.
 *
 * Displays a table with columns for:
 *   - Instance name
 *   - Score obtained by the greedy algorithm
 *   - Baseline score
 *   - Optimal score (with '*' if not proven optimal)
 *   - Percentage gap to baseline
 *   - Percentage gap to optimal
 *
 * @param greedy_scores    Map of instance names to greedy algorithm scores.
 * @param baseline         Map of instance names to baseline scores.
 * @param optimal          Map of instance names to optimal values and flags.
 * @param algorithm_name   Optional label for the greedy algorithm (default "Greedy").
 */
void print_comparison_table(
    const std::unordered_map<std::string, double>& greedy_scores,
    const std::unordered_map<std::string, double>& baseline,
    const std::unordered_map<std::string, EvaluationEntry>& optimal,
    const std::string& algorithm_name = "Greedy"
);

#endif // EVALUATION_LOADER_HPP
