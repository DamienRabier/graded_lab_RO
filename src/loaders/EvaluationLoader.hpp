#ifndef EVALUATION_LOADER_HPP
#define EVALUATION_LOADER_HPP

#include <string>
#include <unordered_map>
#include <vector>

struct EvaluationEntry {
    double value;
    bool is_optimal; // false if there's a '*'
};

std::unordered_map<std::string, EvaluationEntry> load_opt_values(const std::string& filename);
std::unordered_map<std::string, double> load_baseline_values(const std::string& filename);

void print_comparison_table(
    const std::unordered_map<std::string, double>& greedy_scores,
    const std::unordered_map<std::string, double>& baseline,
    const std::unordered_map<std::string, EvaluationEntry>& optimal);

#endif
