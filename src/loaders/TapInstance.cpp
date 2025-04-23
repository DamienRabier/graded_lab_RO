#include "loaders/TapInstance.hpp"
#include <iostream>
#include <iomanip>

/**
 * @brief Constructeur de TapInstance.
 *
 * Initialise les contraintes epsilon et le nom de fichier,
 * puis charge les données d'instance via load_instance().
 *
 * @param filepath Chemin vers le fichier d'instance (.dat)
 * @param eps_t    Contrainte de temps maximale
 * @param eps_d    Contrainte de distance maximale
 */
TapInstance::TapInstance(const std::string& filepath, double eps_t, double eps_d)
    : epsilon_t(eps_t), epsilon_d(eps_d), filename(filepath) {
    // Charger les données (size, interest, time, dist)
    load_instance(filepath);
}

/**
 * @brief Charge une instance TAP depuis un fichier .dat.
 *
 * Lit successivement :
 * 1) le nombre de requêtes (size)
 * 2) la liste des intérêts (interest)
 * 3) la liste des temps (time)
 * 4) la matrice des distances (dist)
 *
 * @param filepath Chemin vers le fichier d'instance
 * @throws std::runtime_error si l'ouverture du fichier échoue
 */
void TapInstance::load_instance(const std::string& filepath) {
    std::ifstream infile(filepath);
    if (!infile) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    // 1) Nombre de requêtes
    infile >> size;

    // 2) Intérêt de chaque requête
    interest.resize(size);
    for (int i = 0; i < size; ++i) {
        infile >> interest[i];
    }

    // 3) Temps d'exécution de chaque requête
    time.resize(size);
    for (int i = 0; i < size; ++i) {
        infile >> time[i];
    }

    // 4) Matrice de distances (size x size)
    dist.assign(size, std::vector<double>(size));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            infile >> dist[i][j];
        }
    }
}

/**
 * @brief Calcule le temps total d'une solution TAP.
 *
 * Additionne les temps d'exécution de toutes les requêtes sélectionnées.
 *
 * @param sol Séquence d'indices de requêtes
 * @return Temps total (double)
 */
double TapInstance::solution_time(const std::vector<int>& sol) const {
    double total = 0.0;
    for (int idx : sol) {
        total += time[idx];
    }
    return total;
}

/**
 * @brief Calcule la distance totale parcourue pour une solution TAP.
 *
 * Parcourt chaque arc entre sol[i] et sol[i+1] et somme les distances.
 *
 * @param sol Séquence d'indices de requêtes
 * @return Distance totale (double)
 */
double TapInstance::solution_distance(const std::vector<int>& sol) const {
    double total = 0.0;
    for (size_t i = 0; i + 1 < sol.size(); ++i) {
        total += dist[sol[i]][sol[i + 1]];
    }
    return total;
}

/**
 * @brief Calcule l'intérêt total d'une solution TAP.
 *
 * Somme des valeurs d'intérêt pour chaque requête dans sol.
 *
 * @param sol Séquence d'indices de requêtes
 * @return Somme des intérêts (double)
 */
double TapInstance::solution_interest(const std::vector<int>& sol) const {
    double total = 0.0;
    for (int idx : sol) {
        total += interest[idx];
    }
    return total;
}

/**
 * @brief Vérifie si une solution respecte les contraintes (temps & distance).
 *
 * @param sol Séquence d'indices de requêtes
 * @return true si solution_time(sol) <= epsilon_t ET solution_distance(sol) <= epsilon_d
 */
bool TapInstance::is_valid_solution(const std::vector<int>& sol) const {
    return solution_time(sol) <= epsilon_t
        && solution_distance(sol) <= epsilon_d;
}

/**
 * @brief Affiche un résumé des caractéristiques de l'instance.
 */
void TapInstance::display_summary() const {
    std::cout << "-- TAP Instance Summary --\n";
    std::cout << "Name      : " << filename       << "\n";
    std::cout << "Size      : " << size           << " queries\n";
    std::cout << "Epsilon T : " << epsilon_t      << "\n";
    std::cout << "Epsilon D : " << epsilon_d      << "\n";
}

/**
 * @brief Affiche la liste des intérêts pour chaque requête.
 */
void TapInstance::display_interest() const {
    std::cout << "Interests: ";
    for (double val : interest) std::cout << val << " ";
    std::cout << "\n";
}

/**
 * @brief Affiche la liste des temps d'exécution pour chaque requête.
 */
void TapInstance::display_time() const {
    std::cout << "Run times: ";
    for (double t : time) std::cout << t << " ";
    std::cout << "\n";
}

/**
 * @brief Affiche la matrice complète des distances entre requêtes.
 */
void TapInstance::display_distance_matrix() const {
    std::cout << "Distance matrix:\n";
    for (const auto& row : dist) {
        for (double d : row) std::cout << std::setw(6) << d << " ";
        std::cout << "\n";
    }
}
