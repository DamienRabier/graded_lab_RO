#include "loaders/TapInstance.hpp"
#include <iostream>
#include <iomanip>

TapInstance::TapInstance(const std::string& filepath, double eps_t, double eps_d)
    : epsilon_t(eps_t), epsilon_d(eps_d), filename(filepath) {
    load_instance(filepath);
}

void TapInstance::load_instance(const std::string& filepath) {
    std::ifstream infile(filepath);
    if (!infile) throw std::runtime_error("Cannot open file: " + filepath);

    infile >> size;

    interest.resize(size);
    for (int i = 0; i < size; ++i) {
        infile >> interest[i];
    }

    time.resize(size);
    for (int i = 0; i < size; ++i) {
        infile >> time[i];
    }

    dist.resize(size, std::vector<double>(size));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            infile >> dist[i][j];
        }
    }
}

double TapInstance::solution_time(const std::vector<int>& sol) const {
    double total = 0.0;
    for (int i : sol) {
        total += time[i];
    }
    return total;
}

double TapInstance::solution_distance(const std::vector<int>& sol) const {
    double total = 0.0;
    for (size_t i = 0; i + 1 < sol.size(); ++i) {
        total += dist[sol[i]][sol[i + 1]];
    }
    return total;
}

double TapInstance::solution_interest(const std::vector<int>& sol) const {
    double total = 0.0;
    for (int i : sol) {
        total += interest[i];
    }
    return total;
}

bool TapInstance::is_valid_solution(const std::vector<int>& sol) const {
    return solution_time(sol) <= epsilon_t && solution_distance(sol) <= epsilon_d;
}

void TapInstance::display_summary() const {
    std::cout << "Size: " << size << "\n";
    std::cout << "Epsilon time: " << epsilon_t << "\n";
    std::cout << "Epsilon distance: " << epsilon_d << "\n";
    std::cout << "Filename: " << filename << "\n";
}

void TapInstance::display_interest() const {
    std::cout << "Interest: ";
    for (const auto& i : interest) {
        std::cout << i << " ";
    }
    std::cout << "\n";
}

void TapInstance::display_time() const {
    std::cout << "Time: ";
    for (const auto& t : time) {
        std::cout << t << " ";
    }
    std::cout << "\n";
}

void TapInstance::display_distance_matrix() const {
    std::cout << "Distance matrix:\n";
    for (const auto& row : dist) {
        for (const auto& d : row) {
            std::cout << d << " ";
        }
        std::cout << "\n";
    }
}
