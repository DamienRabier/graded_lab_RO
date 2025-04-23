#ifndef INSTANCE_LOADER_HPP
#define INSTANCE_LOADER_HPP

#include <string>
#include <vector>
#include "loaders/TapInstance.hpp"

/**
 * @brief Structure décrivant une instance TAP et ses contraintes.
 *
 * Chaque InstanceSpec contient :
 *  - le nom du fichier d'instance
 *  - le budget maximal de temps (epsilon_time)
 *  - le budget maximal de distance (epsilon_distance)
 */
struct InstanceSpec {
    std::string filename;        ///< Nom du fichier de l'instance TAP
    double epsilon_time;         ///< Contrainte de temps maximale (en unités de runtime)
    double epsilon_distance;     ///< Contrainte de distance maximale (en unités de distance)
};

/**
 * @brief Charge les spécifications d'instances TAP depuis un fichier de configuration.
 *
 * Le fichier spécifié doit être un CSV délimité par ';' avec une première ligne d'en-tête.
 * Chaque ligne de données suit le format :
 *     instance_name;epsilon_time;epsilon_distance
 *
 * Cette fonction lit toutes les lignes du fichier (hors en-tête), parse les champs
 * et retourne un vecteur de InstanceSpec à utiliser pour instancier les TAP.
 *
 * @param baseline_file Chemin du fichier contenant les spécifications des instances.
 * @return Un vecteur d'InstanceSpec pour chaque instance listée.
 */
std::vector<InstanceSpec> load_instance_specs(const std::string& baseline_file);

#endif // INSTANCE_LOADER_HPP
