#ifndef INSTANCE_LOADER_HPP
#define INSTANCE_LOADER_HPP

#include <string>
#include <vector>
#include "loaders/TapInstance.hpp"

struct InstanceSpec {
    std::string filename;
    double epsilon_time;
    double epsilon_distance;
};

std::vector<InstanceSpec> load_instance_specs(const std::string& baseline_file);

#endif // INSTANCE_LOADER_HPP
