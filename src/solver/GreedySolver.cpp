#include "solver/GreedySolver.hpp"
#include <vector>
#include <algorithm>
#include <limits>
#include <numeric>
#include <random>

// =============================================================================
//  Helper functions for feasibility checks and distance/insertion computations
// =============================================================================

/**
 * @brief Vérifie si les contraintes temps et distance sont respectées.
 *
 * Permet de s'assurer à chaque étape que l'on ne dépasse pas
 * les budgets epsilon définis pour l'instance TAP.
 *
 * @param t  Temps cumulé actuel
 * @param d  Distance cumulée actuelle
 * @param I  Instance TAP (contient epsilon_t et epsilon_d)
 * @return true si t et d sont inférieurs ou égaux aux epsilons
 */
static inline bool feasible(double t, double d, const TapInstance& I) {
    return t <= I.epsilon_t && d <= I.epsilon_d;
}

/**
 * @brief Calcule la variation de distance si l'on insère la requête q à la position pos de la trajectoire P.
 *
 * Utile pour évaluer le coût additionnel (en distance) de chaque insertion potentielle
 * dans l'heuristique d'insertion gloutonne.
 *
 * @param I    Instance TAP (contient matrice dist)
 * @param P    Séquence actuelle de requêtes
 * @param q    Indice de la requête candidate à insérer
 * @param pos  Position d'insertion dans P (0 = début, P.size() = fin)
 * @return Gain ou perte de distance (delta) dû à l'insertion
 */
static inline double delta_dist_ins(
    const TapInstance& I,
    const std::vector<int>& P,
    int q,
    size_t pos
) {
    const size_t m = P.size();
    if (m == 0) {
        // Pas de déplacement préalable si la trajectoire est vide
        return 0.0;
    }
    if (pos == 0) {
        // Insertion en tête : coût = distance entre q et ancien premier
        return I.dist[q][P[0]];
    } else if (pos == m) {
        // Insertion en fin   : coût = distance entre ancien dernier et q
        return I.dist[P.back()][q];
    } else {
        // Insertion au milieu : coûte la somme des deux nouveaux arcs moins l'ancien arc
        int a = P[pos - 1];
        int b = P[pos];
        return I.dist[a][q] + I.dist[q][b] - I.dist[a][b];
    }
}

// -----------------------------------------------------------------------------
//  1. FastGreedy
//     Heuristique rapide basée sur insertion gloutonne + 2-opt + append tail
// -----------------------------------------------------------------------------

/**
 * @brief Implémente la version "FastGreedy" de l'heuristique.
 *
 * 1) Sélection d'une requête de départ (seed) sur le meilleur ratio intérêt/temps
 * 2) Insertion itérative : à chaque itération, on teste toutes les requêtes non utilisées et leurs
 *    positions d'insertion pour maximiser le ratio intérêt / (temps + distance)
 * 3) Amélioration locale 2-opt sur la séquence pour réduire la distance totale
 * 4) Ajout en fin (tail append) de toutes les requêtes restantes si les contraintes le permettent
 *
 * @param I  Instance TAP à résoudre
 * @return Séquence d'indices représentant la solution gloutonne
 */
static std::vector<int> FastGreedy(const TapInstance& I) {
    const int n = I.size;
    std::vector<bool> used(n, false);    // Marque les requêtes déjà sélectionnées
    std::vector<int> path;               // Séquence courante
    double T = 0.0, D = 0.0;             // Temps et distance cumulés

    // 1.a) Seed : choisir la meilleure requête initiale sur le ratio interest/time
    int seed = -1;
    double bestR = -1.0;
    for (int i = 0; i < n; ++i) {
        if (I.time[i] > I.epsilon_t) continue;  // Filtrer les requêtes impossibles dès le départ
        double r = I.interest[i] / I.time[i];
        if (r > bestR) {
            bestR = r;
            seed = i;
        }
    }
    if (seed < 0) {
        // Aucune requête n'est faisable, on retourne une solution vide
        return path;
    }
    // Initialisation de la trajectoire avec le seed
    path.push_back(seed);
    used[seed] = true;
    T = I.time[seed];

    // 1.b) Insertion gloutonne : on recherche l'insertion offrant le meilleur gain au ratio
    bool progress = true;
    while (progress) {
        progress = false;
        int bestQ = -1;
        int bestPos = -1;
        double bestGain = -1.0;
        double bestDT = 0.0, bestDD = 0.0;

        // Tester chaque requête non utilisée
        for (int q = 0; q < n; ++q) {
            if (used[q]) continue;
            // Tester toutes les positions d'insertion possibles
            for (size_t pos = 0; pos <= path.size(); ++pos) {
                double dd = delta_dist_ins(I, path, q, pos);
                double dt = I.time[q];
                // Vérifier la faisabilité après insertion
                if (!feasible(T + dt, D + dd, I)) continue;
                double cost = dt + dd + 1e-9; // Petite constante pour éviter div/0
                double gain = I.interest[q] / cost;
                if (gain > bestGain) {
                    bestGain = gain;
                    bestQ = q;
                    bestPos = (int)pos;
                    bestDT = dt;
                    bestDD = dd;
                }
            }
        }
        if (bestQ != -1) {
            // Insérer la meilleure requête trouvée
            path.insert(path.begin() + bestPos, bestQ);
            used[bestQ] = true;
            T += bestDT;
            D += bestDD;
            progress = true;
        }
    }

    // 1.c) Amélioration locale 2-opt pour réduire la distance
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 0; i + 2 < path.size() && !improved; ++i) {
            for (size_t j = i + 2; j < path.size() && !improved; ++j) {
                int a = path[i], b = path[i + 1];
                int c = path[j - 1], d = path[j];
                // Calculer variation si on échange [b..c]
                double delta = I.dist[a][c] + I.dist[b][d] 
                             - I.dist[a][b] - I.dist[c][d];
                // Si amélioration et toujours faisable
                if (delta < -1e-6 && feasible(T, D + delta, I)) {
                    std::reverse(path.begin() + i + 1, path.begin() + j);
                    D += delta;
                    improved = true;
                }
            }
        }
    }

    // 1.d) Append tail : essayer d'ajouter les requêtes restantes en fin de parcours
    int last = path.back();
    for (int q = 0; q < n; ++q) {
        if (used[q]) continue;
        double dd = I.dist[last][q];
        double dt = I.time[q];
        if (feasible(T + dt, D + dd, I)) {
            path.push_back(q);
            used[q] = true;
            T += dt;
            D += dd;
            last = q;
        }
    }

    return path;
}

// -----------------------------------------------------------------------------
//  2. EnhancedGreedy
//     Variante normalisée puis fallback sur baseline en cas de besoin
// -----------------------------------------------------------------------------



/**
 * @brief Représentation de chemin intermédiaire pour EnhancedGreedy.
 *
 * Stocke la trajectoire, les flags d'usage, et les métriques cumulées.
 */
struct Path {
    std::vector<int> nodes;         ///< Séquence de requêtes
    std::vector<bool> used;         ///< Requêtes déjà insérées
    double time = 0.0;              ///< Temps cumulé
    double dist = 0.0;              ///< Distance cumulée
    double reward = 0.0;            ///< Intérêt total accumulé
    explicit Path(int n) : used(n, false) {}
};

/**
 * @brief Tente l'insertion "best-normalized-cost" de q dans P.
 *
 * Calcule pour chaque position un score normalisé par epsilon.
 * Si insertion faisable, met à jour P et renvoie true.
 */
static bool try_best_insert(
    Path& P,
    int q,
    const TapInstance& I,
    double wT,
    double wD
) {
    size_t m = P.nodes.size();
    size_t bestPos = m + 1;
    double bestScore = -1.0;
    double bestDT = 0.0, bestDD = 0.0;

    // Parcourir toutes les positions pour trouver la meilleure insertion
    for (size_t pos = 0; pos <= m; ++pos) {
        double dd = delta_dist_ins(I, P.nodes, q, pos);
        double dt = I.time[q];
        if (!feasible(P.time + dt, P.dist + dd, I)) continue;
        double cost = dt * wT + dd * wD;
        if (cost <= 0) cost = 1e-9;
        double score = I.interest[q] / cost;
        if (score > bestScore) {
            bestScore = score;
            bestPos = pos;
            bestDT = dt;
            bestDD = dd;
        }
    }

    // Si une position valable a été trouvée, on insère
    if (bestPos <= m) {
        P.nodes.insert(P.nodes.begin() + bestPos, q);
        P.used[q]   = true;
        P.time     += bestDT;
        P.dist     += bestDD;
        P.reward   += I.interest[q];
        return true;
    }
    return false;
}

/**
 * @brief Implémente la stratégie 2-opt sur le Path P.
 */
static void two_opt(Path& P, const TapInstance& I) {
    int m = (int)P.nodes.size();
    if (m < 4) return;  // Pas assez de points pour 2-opt
    bool improved = true;
    while (improved) {
        improved = false;
        for (int i = 0; i < m - 3 && !improved; ++i) {
            for (int k = i + 2; k < m - 1 && !improved; ++k) {
                int a = P.nodes[i], b = P.nodes[i + 1];
                int c = P.nodes[k], d = P.nodes[k + 1];
                double before = I.dist[a][b] + I.dist[c][d];
                double after  = I.dist[a][c] + I.dist[b][d];
                double delta  = after - before;
                if (delta < -1e-9 && feasible(P.time, P.dist + delta, I)) {
                    std::reverse(P.nodes.begin() + i + 1, P.nodes.begin() + k + 1);
                    P.dist += delta;
                    improved = true;
                }
            }
        }
    }
}

/**
 * @brief Génère un chemin de fallback simple basé sur la baseline (tri par intérêt).
 */
static Path baseline_path(const TapInstance& I) {
    int n = I.size;
    Path B(n);
    std::vector<int> idx(n);
    std::iota(idx.begin(), idx.end(), 0);
    // Sorte par intérêt décroissant
    std::sort(idx.begin(), idx.end(), [&](int a, int b) {
        return I.interest[a] > I.interest[b];
    });
    // Ajoute tant que faisable
    for (int q : idx) {
        if (B.nodes.empty()) {
            if (I.time[q] <= I.epsilon_t) {
                B.nodes.push_back(q);
                B.used[q]   = true;
                B.time      = I.time[q];
                B.reward    = I.interest[q];
            }
            continue;
        }
        int last = B.nodes.back();
        double dt = I.time[q];
        double dd = I.dist[last][q];
        if (feasible(B.time + dt, B.dist + dd, I)) {
            B.nodes.push_back(q);
            B.used[q]   = true;
            B.time     += dt;
            B.dist     += dd;
            B.reward   += I.interest[q];
        }
    }
    return B;
}

/**
 * @brief Implémente la version "EnhancedGreedy" de l'heuristique.
 *
 * Normalise les coûts (temps vs distance), applique une boucle d'insertion,
 * un passage 2-opt, puis compare avec la baseline pour garantir un fallback performant.
 */
static std::vector<int> EnhancedGreedy(const TapInstance& I) {
    const int n = I.size;
    if (n == 0) return {};

    // Coefficients pour normalisation des coûts
    const double wT = 1.0 / I.epsilon_t;
    const double wD = 1.0 / I.epsilon_d;

    // Seed initial sur meilleur ratio intérêt/temps
    int seed = 0;
    double best = -1.0;
    for (int i = 0; i < n; ++i) {
        double s = I.interest[i] / I.time[i];
        if (s > best) {
            best = s;
            seed = i;
        }
    }
    Path P(n);
    P.nodes.push_back(seed);
    P.used[seed]  = true;
    P.time        = I.time[seed];
    P.reward      = I.interest[seed];

    // Boucle principale d'insertion normalisée
    bool progress = true;
    while (progress) {
        progress = false;
        int bestQ = -1;
        double bestScore = -1.0;
        size_t bestPos = 0;
        double bestDT = 0.0, bestDD = 0.0;
        for (int q = 0; q < n; ++q) {
            if (P.used[q]) continue;
            size_t m = P.nodes.size();
            for (size_t pos = 0; pos <= m; ++pos) {
                double dd = delta_dist_ins(I, P.nodes, q, pos);
                double dt = I.time[q];
                if (!feasible(P.time + dt, P.dist + dd, I)) continue;
                double cost = dt * wT + dd * wD;
                if (cost <= 0) cost = 1e-9;
                double score = I.interest[q] / cost;
                if (score > bestScore) {
                    bestScore = score;
                    bestQ     = q;
                    bestPos   = pos;
                    bestDT    = dt;
                    bestDD    = dd;
                }
            }
        }
        if (bestQ != -1) {
            P.nodes.insert(P.nodes.begin() + bestPos, bestQ);
            P.used[bestQ] = true;
            P.time       += bestDT;
            P.dist       += bestDD;
            P.reward     += I.interest[bestQ];
            progress      = true;
        }
    }

    // Passage sur les requêtes restantes triées par intérêt décroissant
    std::vector<int> rest;
    rest.reserve(n);
    for (int i = 0; i < n; ++i) {
        if (!P.used[i]) rest.push_back(i);
    }
    std::sort(rest.begin(), rest.end(), [&](int a, int b) {
        return I.interest[a] > I.interest[b];
    });
    for (int q : rest) {
        try_best_insert(P, q, I, wT, wD);
    }

    // Amélioration 2-opt de la trajectoire
    two_opt(P, I);

    // Comparaison avec la baseline et fallback si nécessaire
    Path B = baseline_path(I);
    if (B.reward > P.reward) {
        P = std::move(B);
    }

    return P.nodes;
}

// =============================================================================
//  Public entry point – exécute les deux heuristiques et choisit la meilleure
// =============================================================================

std::vector<int> greedy_solve(const TapInstance& inst) {
    // Exécution de FastGreedy
    std::vector<int> path1 = FastGreedy(inst);
    double val1 = inst.solution_interest(path1);

    // Exécution d'EnhancedGreedy
    std::vector<int> path2 = EnhancedGreedy(inst);
    double val2 = inst.solution_interest(path2);

    // Retourner la solution la plus intéressante
    return (val2 > val1) ? path2 : path1;
}
