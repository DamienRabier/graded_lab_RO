#ifndef TAP_INSTANCE_HPP
#define TAP_INSTANCE_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <numeric>

class TapInstance {
public:
    int size;
    std::vector<double> interest;
    std::vector<double> time;
    std::vector<std::vector<double>> dist;
    double epsilon_t;
    double epsilon_d;
    std::string filename;

    TapInstance(const std::string& filepath, double eps_t, double eps_d);

    double solution_time(const std::vector<int>& sol) const;
    double solution_distance(const std::vector<int>& sol) const;
    double solution_interest(const std::vector<int>& sol) const;
    bool is_valid_solution(const std::vector<int>& sol) const;

    void display_summary() const;
    void display_interest() const;
    void display_time() const;
    void display_distance_matrix() const;

private:
    void load_instance(const std::string& filepath);
};

#endif // TAP_INSTANCE_HPP
