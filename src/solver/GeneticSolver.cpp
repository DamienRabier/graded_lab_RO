// solver/GeneticSolver.cpp
#include "solver/GeneticSolver.hpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

// Pénalité pour solution invalide
static constexpr double INVALID_FITNESS = -1e9;

std::vector<int> genetic_solve(const TapInstance& instance) {
    int n = instance.size;

    // GA parameters (you can tune these)
    const int POP_SIZE          = 1000;
    const int GENERATIONS       = 200;
    const double CROSSOVER_RATE = 0.8;
    const double MUTATION_RATE  = 0.2;
    const int TOURNAMENT_SIZE   = 10;

    std::mt19937 rng(std::random_device{}());

    // Utility: decode a full permutation into a feasible TAP path
    auto decode = [&](const std::vector<int>& perm) {
        std::vector<int> sol;
        sol.reserve(n);
        double total_time = 0.0, total_dist = 0.0;
        int last = -1;
        for (int idx : perm) {
            double travel = (last >= 0 ? instance.dist[last][idx] : 0.0);
            if (total_time + instance.time[idx] > instance.epsilon_t ||
                total_dist + travel             > instance.epsilon_d)
                break;
            sol.push_back(idx);
            total_time += instance.time[idx];
            total_dist += travel;
            last = idx;
        }
        return sol;
    };

    // Evaluate fitness = total interest of decoded path,
    // OR INVALID_FITNESS si la solution n'est pas valide ou est vide
    auto evaluate = [&](const std::vector<int>& perm) {
        auto sol = decode(perm);
        if (sol.empty() || !instance.is_valid_solution(sol)) {
            return INVALID_FITNESS;
        }
        double score = 0.0;
        for (int i : sol) score += instance.interest[i];
        return score;
    };

    // --- 1) Initialize population with random permutations ---
    std::vector<int> base_perm(n);
    std::iota(base_perm.begin(), base_perm.end(), 0);

    std::vector<std::vector<int>> population;
    population.reserve(POP_SIZE);
    std::vector<double> fitnesses(POP_SIZE);

    for (int i = 0; i < POP_SIZE; ++i) {
        std::vector<int> perm = base_perm;
        std::shuffle(perm.begin(), perm.end(), rng);
        population.push_back(perm);
        fitnesses[i] = evaluate(perm);
    }

    // Track best
    std::vector<int> best_perm = population[0];
    double best_fitness = fitnesses[0];
    for (int i = 1; i < POP_SIZE; ++i) {
        if (fitnesses[i] > best_fitness) {
            best_fitness = fitnesses[i];
            best_perm    = population[i];
        }
    }

    std::uniform_real_distribution<double> real_dist(0.0, 1.0);
    std::uniform_int_distribution<int> pop_idx_dist(0, POP_SIZE - 1);

    // --- 2) Main GA loop ---
    for (int gen = 0; gen < GENERATIONS; ++gen) {
        std::vector<std::vector<int>> new_pop;
        new_pop.reserve(POP_SIZE);

        // Tournament selection helper
        auto tournament_select = [&]() {
            int best = pop_idx_dist(rng);
            double best_fit = fitnesses[best];
            for (int k = 1; k < TOURNAMENT_SIZE; ++k) {
                int idx = pop_idx_dist(rng);
                if (fitnesses[idx] > best_fit) {
                    best = idx;
                    best_fit = fitnesses[idx];
                }
            }
            return population[best];
        };

        // Build new population
        while ((int)new_pop.size() < POP_SIZE) {
            auto parent1 = tournament_select();
            auto parent2 = tournament_select();
            std::vector<int> child1 = parent1, child2 = parent2;

            // Crossover: Order Crossover (OX)
            if (real_dist(rng) < CROSSOVER_RATE) {
                std::uniform_int_distribution<int> cut_dist(0, n-1);
                int a = cut_dist(rng), b = cut_dist(rng);
                if (a > b) std::swap(a, b);

                // child1 from parent1 + parent2
                {
                    std::vector<bool> used(n, false);
                    for (int i = a; i <= b; ++i) {
                        child1[i] = parent1[i];
                        used[ child1[i] ] = true;
                    }
                    int pos = (b+1) % n;
                    for (int i = 0; i < n; ++i) {
                        int c = parent2[(b+1+i) % n];
                        if (!used[c]) {
                            child1[pos] = c;
                            used[c] = true;
                            pos = (pos+1) % n;
                        }
                    }
                }
                // child2 from parent2 + parent1
                {
                    std::vector<bool> used(n, false);
                    for (int i = a; i <= b; ++i) {
                        child2[i] = parent2[i];
                        used[ child2[i] ] = true;
                    }
                    int pos = (b+1) % n;
                    for (int i = 0; i < n; ++i) {
                        int c = parent1[(b+1+i) % n];
                        if (!used[c]) {
                            child2[pos] = c;
                            used[c] = true;
                            pos = (pos+1) % n;
                        }
                    }
                }
            }

            // Mutation: simple swap
            auto mutate = [&](std::vector<int>& c) {
                std::uniform_int_distribution<int> idx_dist(0, n-1);
                int i = idx_dist(rng), j = idx_dist(rng);
                std::swap(c[i], c[j]);
            };
            if (real_dist(rng) < MUTATION_RATE) mutate(child1);
            if (real_dist(rng) < MUTATION_RATE) mutate(child2);

            new_pop.push_back(std::move(child1));
            if ((int)new_pop.size() < POP_SIZE)
                new_pop.push_back(std::move(child2));
        }

        // Replace and re-evaluate
        population = std::move(new_pop);
        for (int i = 0; i < POP_SIZE; ++i) {
            fitnesses[i] = evaluate(population[i]);
            if (fitnesses[i] > best_fitness) {
                best_fitness = fitnesses[i];
                best_perm    = population[i];
            }
        }
    }

    // Decode and return the best individual found
    return decode(best_perm);
}
