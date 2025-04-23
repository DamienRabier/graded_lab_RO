// solver/GeneticSolver.hpp
#ifndef GENETIC_SOLVER_HPP
#define GENETIC_SOLVER_HPP

#include "loaders/TapInstance.hpp"
#include <vector>

/**
 * genetic_solve
 *
 * Runs a simple genetic algorithm to solve the Travelling Analyst Problem (TAP).
 * Each individual is a permutation of all query indices; feasible paths are
 * obtained by decoding prefixes until the time or distance budget is exceeded.
 *
 * @param instance  The TAP instance to solve.
 * @return          A sequence of query indices representing the best-found path.
 */
std::vector<int> genetic_solve(const TapInstance& instance);

#endif // GENETIC_SOLVER_HPP
