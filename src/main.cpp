#include "loaders/TapInstance.hpp"
#include "solver/GreedySolver.hpp"
#include "loaders/InstanceLoader.hpp"
#include "loaders/EvaluationLoader.hpp"
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <iomanip>
#include <unordered_map>

int main() {
    // Check current working directory, used for debugging only
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "Current working directory: " << cwd << std::endl;
    } else {
        perror("getcwd() error");
    }

    std::string baseline_file = "instances/baselines.txt";
    std::string instance_dir = "instances/";
    std::string baseline_result_file = "instances/baselines_results.txt";
    std::string optimal_result_file = "instances/opt.txt";

    // Charger spécifications des instances (nom + epsilon)
    auto specs = load_instance_specs(baseline_file);

    // Résultats de l'algo greedy
    std::unordered_map<std::string, double> greedy_scores;

    std::cout << "=== Greedy Solver for all instances ===\n";
    for (const auto& spec : specs) {
        std::string path = instance_dir + spec.filename;
        TapInstance instance(path, spec.epsilon_time, spec.epsilon_distance);
        std::vector<int> solution = greedy_solve(instance);
        double interest = instance.solution_interest(solution);

        greedy_scores[spec.filename] = interest;

        std::cout << "Instance: " << spec.filename << "\n";
        std::cout << "  Valid: " << instance.is_valid_solution(solution) << "\n";
        std::cout << "  Interest: " << interest << "\n";
        std::cout << "  Time: " << instance.solution_time(solution) << "\n";
        std::cout << "  Distance: " << instance.solution_distance(solution) << "\n";
        std::cout << "  Solution: ";
        for (int i : solution) std::cout << i << " ";
        std::cout << "\n\n";
    }

    std::cout << "=== Résumé comparatif ===\n";

    std::cout << " --- Greedy vs Baseline vs Optimal ---\n";
    auto baseline_values = load_baseline_values(baseline_result_file);
    auto optimal_values = load_opt_values(optimal_result_file);

    std::string algo_name = "Greedy";
    print_comparison_table(greedy_scores, baseline_values, optimal_values, algo_name);
    std::cout << "=== Résumé comparatif terminé ===\n";

    return 0;
}
