#include "loaders/EvaluationLoader.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

/**
 * @file EvaluationLoader.cpp
 * @brief Implémentation des fonctions de chargement et d'affichage des résultats d'évaluation
 *
 * Ce fichier fournit :
 * - load_opt_values : lecture des valeurs optimales (ou bornes) dans un fichier CSV
 * - load_baseline_values : lecture des valeurs baseline dans un fichier CSV
 * - print_comparison_table : affichage d'un tableau comparatif formaté
 */

std::unordered_map<std::string, EvaluationEntry>
load_opt_values(const std::string& filename) {
    // Résultat : map instance_name -> EvaluationEntry
    std::unordered_map<std::string, EvaluationEntry> result;
    std::ifstream file(filename);
    std::string line;

    // Vérification de l'ouverture du fichier
    if (!file.is_open()) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier d'optimum '"
                  << filename << "'\n";
        return result;  // retourne map vide
    }

    // Lire et ignorer la première ligne (en-tête)
    std::getline(file, line);

    // Traiter chaque ligne du fichier
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name, value_str;

        // Extraction du nom d'instance et de la valeur (avec '*' éventuel)
        if (!std::getline(iss, name, ';') || !std::getline(iss, value_str, ';')) {
            std::cerr << "Avertissement : ligne mal formée -> '" << line << "'\n";
            continue;  // passe à la ligne suivante
        }

        EvaluationEntry entry;
        // Détecter la présence du '*' indiquant un non-optimum prouvé
        if (!value_str.empty() && value_str.back() == '*') {
            entry.is_optimal = false;
            value_str.pop_back();  // retirer le caractère '*'
        } else {
            entry.is_optimal = true;
        }

        // Conversion de la chaîne numérique en double
        try {
            entry.value = std::stod(value_str);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Erreur : valeur invalide pour '" << name
                      << "' -> '" << value_str << "'\n";
            continue;  // ignorer cette entrée
        }

        // Stockage dans la map
        result[name] = entry;
    }

    return result;
}

std::unordered_map<std::string, double>
load_baseline_values(const std::string& filename) {
    // Résultat : map instance_name -> baseline_value
    std::unordered_map<std::string, double> result;
    std::ifstream file(filename);
    std::string line;

    // Lire et ignorer la ligne d'en-tête
    std::getline(file, line);

    // Parcourir chaque ligne du fichier
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name, val_str;

        // Extraction du nom et de la valeur baseline
        std::getline(iss, name, ';');
        std::getline(iss, val_str, ';');

        // Conversion en double et insertion
        result[name] = std::stod(val_str);
    }

    return result;
}

void print_comparison_table(
    const std::unordered_map<std::string, double>& greedy,
    const std::unordered_map<std::string, double>& baseline,
    const std::unordered_map<std::string, EvaluationEntry>& opt,
    const std::string& algorithm_name
) {
    // En-tête du tableau
    std::cout << std::setw(20) << "Instance"
              << std::setw(12) << algorithm_name
              << std::setw(12) << "Baseline"
              << std::setw(12) << "Optimal"
              << std::setw(12) << "ΔBase(%)"
              << std::setw(12) << "ΔOpt(%)" << "\n";

    // Pour chaque résultat de l'algorithme glouton
    for (const auto& [name, gval] : greedy) {
        // Vérifier la présence des valeurs baseline et optimale
        if (baseline.find(name) == baseline.end()) {
            std::cerr << "Erreur : '" << name
                      << "' introuvable dans baseline values\n";
            continue;
        }
        if (opt.find(name) == opt.end()) {
            std::cerr << "Erreur : '" << name
                      << "' introuvable dans optimal values\n";
            continue;
        }

        double bval = baseline.at(name);
        const auto& entry = opt.at(name);
        double oval = entry.value;
        bool is_opt = entry.is_optimal;

        // Calcul des écarts en pourcentage
        double gap_base = 100.0 * (bval - gval) / oval;
        double gap_opt  = 100.0 * (oval - gval) / oval;

        // Affichage formaté de la ligne
        std::cout << std::setw(20) << name
                  << std::setw(12) << std::fixed << std::setprecision(2) << gval
                  << std::setw(12) << bval
                  << std::setw(12)
                     // Marquer d'un '*' si non optimal prouvé
                     << (is_opt ? std::to_string(oval)
                                : std::to_string(oval) + "*")
                  << std::setw(12) << gap_base
                  << std::setw(12) << gap_opt
                  << "\n";
    }
}