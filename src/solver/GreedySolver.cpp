#include "solver/GreedySolver.hpp"
#include <vector>
#include <limits>

std::vector<int> greedy_solve(const TapInstance& instance) {
    int n = instance.size;
    std::vector<bool> used(n, false);
    std::vector<int> solution;
    double total_time = 0.0;
    double total_dist = 0.0;
    int last = -1;  // index of last added query, -1 if none

    // 1) Incremental greedy: at each étape, pick the unused query i
    //    maximizing interest[i] / (time[i] + dist[last][i])
    while (true) {
        int best = -1;
        double best_score = -1.0;

        for (int i = 0; i < n; ++i) {
            if (used[i]) continue;

            double travel = (last >= 0 ? instance.dist[last][i] : 0.0);
            double dtotal = total_time + instance.time[i];
            double ddist  = total_dist + travel;

            // respecter les contraintes
            if (dtotal > instance.epsilon_t)   continue;
            if (ddist  > instance.epsilon_d)   continue;

            // ratio intérêt / coût marginal
            double cost  = instance.time[i] + travel;
            double score = instance.interest[i] / cost;

            if (score > best_score) {
                best_score = score;
                best       = i;
            }
        }

        if (best < 0) break;  // plus aucun choix valide
        // ajouter la requête best
        used[best]     = true;
        solution.push_back(best);
        total_time    += instance.time[best];
        if (last >= 0) total_dist += instance.dist[last][best];
        last           = best;
    }

    // 2) Post-traitement simple (1-opt insertion) :
    //    tenter d’insérer toute requête unused en fin de parcours si ça rentre
    for (int i = 0; i < n; ++i) {
        if (used[i]) continue;
        double travel = (last >= 0 ? instance.dist[last][i] : 0.0);
        if (total_time + instance.time[i] <= instance.epsilon_t &&
            total_dist  + travel             <= instance.epsilon_d) {
            solution.push_back(i);
            used[i]      = true;
            total_time  += instance.time[i];
            total_dist  += travel;
            last         = i;
        }
    }

    return solution;
}
