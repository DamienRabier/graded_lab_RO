#include "solver/GreedySolver.hpp"
#include <numeric>
#include <algorithm>

std::vector<int> greedy_solve(const TapInstance& instance) {
    std::vector<int> indices(instance.size);
    std::iota(indices.begin(), indices.end(), 0);

    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return instance.interest[a] > instance.interest[b];
    });

    std::vector<int> solution;
    double total_time = 0;
    double total_dist = 0;

    for (int i : indices) {
        double time_i = total_time + instance.time[i];
        double dist_i = solution.empty() ? 0 : total_dist + instance.dist[solution.back()][i];

        if (time_i <= instance.epsilon_t && dist_i <= instance.epsilon_d) {
            solution.push_back(i);
            total_time = time_i;
            total_dist = dist_i;
        }
    }

    return solution;
}
