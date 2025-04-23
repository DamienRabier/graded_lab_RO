#include "loaders/InstanceLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<InstanceSpec> load_instance_specs(const std::string& baseline_file) {
    std::vector<InstanceSpec> instances;
    std::ifstream file(baseline_file);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Unable to open " << baseline_file << "\n";
        return instances;
    }

    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        InstanceSpec spec;

        std::getline(iss, spec.filename, ';');
        std::getline(iss, token, ';');
        spec.epsilon_time = std::stod(token);
        std::getline(iss, token, ';');
        spec.epsilon_distance = std::stod(token);

        instances.push_back(spec);
    }

    return instances;
}
