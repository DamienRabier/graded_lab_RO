#ifndef TAP_INSTANCE_HPP
#define TAP_INSTANCE_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <numeric>

/**
 * @brief Représente une instance du Travelling Analyst Problem (TAP).
 *
 * Contient les données d'une instance :
 *   - le nombre de requêtes (size)
 *   - l'intérêt, le temps d'exécution, et la matrice de distances
 *   - les contraintes epsilon pour le temps et la distance
 *   - le nom du fichier source
 */
class TapInstance {
public:
    int size;                              ///< Nombre de requêtes dans l'instance
    std::vector<double> interest;         ///< Liste des valeurs d'intérêt pour chaque requête
    std::vector<double> time;             ///< Liste des temps d'exécution pour chaque requête
    std::vector<std::vector<double>> dist;///< Matrice des distances entre chaque paire de requêtes
    double epsilon_t;                     ///< Contrainte de temps maximale (budget)
    double epsilon_d;                     ///< Contrainte de distance maximale (budget)
    std::string filename;                 ///< Chemin du fichier d'instance chargé

    /**
     * @brief Constructeur principal, charge une instance depuis un fichier.
     *
     * @param filepath Chemin vers le fichier d'instance .dat
     * @param eps_t    Contrainte maximale de temps
     * @param eps_d    Contrainte maximale de distance
     * @throws std::runtime_error si le fichier ne peut pas être ouvert ou lu
     */
    TapInstance(const std::string& filepath, double eps_t, double eps_d);

    /**
     * @brief Calcule le temps total d'une solution donnée.
     *
     * @param sol Séquence d'indices de requêtes formant la solution
     * @return Temps total cumulé des requêtes dans la solution
     */
    double solution_time(const std::vector<int>& sol) const;

    /**
     * @brief Calcule la distance totale parcourue par une solution.
     *
     * Parcourt la séquence sol[i] -> sol[i+1] et somme les distances.
     *
     * @param sol Séquence d'indices de requêtes
     * @return Distance totale parcourue
     */
    double solution_distance(const std::vector<int>& sol) const;

    /**
     * @brief Calcule l'intérêt total d'une solution.
     *
     * Somme des valeurs d'intérêt des requêtes sélectionnées.
     *
     * @param sol Séquence d'indices de requêtes
     * @return Somme des intérêts
     */
    double solution_interest(const std::vector<int>& sol) const;

    /**
     * @brief Vérifie si une solution respecte les contraintes epsilon.
     *
     * @param sol Séquence d'indices de requêtes
     * @return true si le temps et la distance sont inférieurs ou égaux aux epsilons
     */
    bool is_valid_solution(const std::vector<int>& sol) const;

    /**
     * @brief Affiche un résumé de l'instance (taille, epsilons, nom de fichier).
     */
    void display_summary() const;

    /**
     * @brief Affiche la liste des valeurs d'intérêt.
     */
    void display_interest() const;

    /**
     * @brief Affiche la liste des temps d'exécution.
     */
    void display_time() const;

    /**
     * @brief Affiche la matrice des distances.
     */
    void display_distance_matrix() const;

private:
    /**
     * @brief Charge les données d'instance depuis un fichier .dat.
     *
     * Lit le nombre de requêtes, puis les listes interest et time,
     * puis la matrice de distances.
     *
     * @param filepath Chemin vers le fichier .dat
     * @throws std::runtime_error si le fichier ne peut pas être ouvert
     */
    void load_instance(const std::string& filepath);
};

#endif // TAP_INSTANCE_HPP
