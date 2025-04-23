#ifndef GREEDY_SOLVER_HPP
#define GREEDY_SOLVER_HPP

#include "loaders/TapInstance.hpp"
#include <vector>

/**
 * @brief Résout une instance du Travelling Analyst Problem (TAP)
 *        à l'aide d'une heuristique gloutonne.
 *
 * @param instance Référence constante à l'instance TAP à résoudre.
 * @return Séquence d'indices de requêtes formant la solution gloutonne.
 */
std::vector<int> greedy_solve(const TapInstance& instance);

#endif // GREEDY_SOLVER_HPP
