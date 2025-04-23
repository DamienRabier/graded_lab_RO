#include "loaders/InstanceLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @brief Charge les spécifications des instances TAP depuis un fichier CSV.
 *
 * Lit chaque ligne (hors en-tête) du fichier donné, parse le nom de l'instance
 * et ses contraintes epsilon (temps et distance), et stocke le tout dans un vecteur.
 *
 * @param baseline_file Chemin vers le fichier CSV :
 *        instance_name;epsilon_time;epsilon_distance
 * @return Un vecteur d'InstanceSpec, un élément par ligne de données.
 */
std::vector<InstanceSpec> load_instance_specs(const std::string& baseline_file) {
    std::vector<InstanceSpec> instances;  // Contiendra toutes les spécifications chargées
    std::ifstream file(baseline_file);
    std::string line;

    // Vérifier l'ouverture du fichier
    if (!file.is_open()) {
        std::cerr << "Unable to open specs file: " << baseline_file << "\n";
        return instances;  // Retourner un vecteur vide en cas d'erreur
    }

    // Ignorer la première ligne d'en-tête
    std::getline(file, line);

    // Parcourir chaque ligne de données
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        InstanceSpec spec;
        std::string token;

        // Extraire le nom de fichier (jusqu'au premier ';')
        std::getline(iss, spec.filename, ';');

        // Extraire et convertir la contrainte de temps
        if (std::getline(iss, token, ';')) {
            spec.epsilon_time = std::stod(token);
        } else {
            std::cerr << "Missing time epsilon for line: " << line << "\n";
            continue;  // Ligne mal formée -> passer à la suivante
        }

        // Extraire et convertir la contrainte de distance
        if (std::getline(iss, token, ';')) {
            spec.epsilon_distance = std::stod(token);
        } else {
            std::cerr << "Missing distance epsilon for line: " << line << "\n";
            continue;  // Ligne mal formée -> passer à la suivante
        }

        // Ajouter la spécification parsée au vecteur
        instances.push_back(spec);
    }

    return instances;
}
