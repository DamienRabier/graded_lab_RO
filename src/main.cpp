#include "loaders/TapInstance.hpp"
#include "solver/GreedySolver.hpp"
#include "loaders/InstanceLoader.hpp"
#include "loaders/EvaluationLoader.hpp"

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <limits.h>
#include <unordered_map>

// main loop
int main() {
    // [Debug only] Affiche le répertoire de travail courant
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "Répertoire de travail : " << cwd << "\n\n";
    } else {
        perror("Erreur getcwd()");
    }

    // paths, doit etre dans graded_lab/...
    const std::string instance_directory = "instances/";
    const std::string spec_file = instance_directory + "baselines.txt";
    const std::string baseline_result_file = instance_directory + "baselines_results.txt";
    const std::string optimal_result_file = instance_directory + "opt.txt";

    // Charger les spécifications des instances (fichier + epsilon temps/distance)
    auto specs = load_instance_specs(spec_file);

    // Dictionnaire pour stocker les scores de l'algo greedy
    std::unordered_map<std::string, double> greedy_scores;

    std::cout << "=== Résolution gloutonne pour chaque instance ===\n";
    for (const auto& spec : specs) {
        std::string full_path = instance_directory + spec.filename;

        // Chargement de l'instance TAP
        TapInstance instance(full_path, spec.epsilon_time, spec.epsilon_distance);

        // Exécution de l'algorithme glouton
        std::vector<int> solution = greedy_solve(instance);
        double interest = instance.solution_interest(solution);
        greedy_scores[spec.filename] = interest;

        // Affichage des résultats pour l'instance
        std::cout << "Instance : " << spec.filename << "\n";
        std::cout << "  Solution valide     : " << std::boolalpha << instance.is_valid_solution(solution) << "\n";
        std::cout << "  Intérêt total       : " << interest << "\n";
        std::cout << "  Temps total         : " << instance.solution_time(solution) << "\n";
        std::cout << "  Distance totale     : " << instance.solution_distance(solution) << "\n";
        std::cout << "  Séquence sélectionnée : ";
        for (int id : solution) std::cout << id << " ";
        std::cout << "\n\n";
    }

    // Comparaison finale avec baseline et optima connus
    std::cout << "=== Résumé comparatif ===\n";
    auto baseline_values = load_baseline_values(baseline_result_file);
    auto optimal_values  = load_opt_values(optimal_result_file);
    print_comparison_table(greedy_scores, baseline_values, optimal_values);
    std::cout << "=== Fin du résumé ===\n";

    return 0;
}
