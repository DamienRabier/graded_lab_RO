#include "loaders/EvaluationLoader.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

std::unordered_map<std::string, EvaluationEntry> load_opt_values(const std::string& filename) {
    std::unordered_map<std::string, EvaluationEntry> result;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << "\n";
        return result;
    }

    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name, value_str;

        // Parse the line
        if (!std::getline(iss, name, ';') || !std::getline(iss, value_str, ';')) {
            std::cerr << "Malformed line: " << line << "\n";
            continue; // Skip this line
        }

        // Handle the '*' character
        EvaluationEntry entry;
        if (!value_str.empty() && value_str.back() == '*') {
            value_str.pop_back();
            entry.is_optimal = false;
        } else {
            entry.is_optimal = true;
        }

        // Convert the value to double
        try {
            entry.value = std::stod(value_str);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid value for instance '" << name << "': " << value_str << "\n";
            continue; // Skip this line
        }

        result[name] = entry;
    }

    return result;
}

std::unordered_map<std::string, double> load_baseline_values(const std::string& filename) {
    std::unordered_map<std::string, double> result;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name, val;
        std::getline(iss, name, ';');
        std::getline(iss, val, ';');
        result[name] = std::stod(val);
    }
    return result;
}

void print_comparison_table(
    const std::unordered_map<std::string, double>& greedy,
    const std::unordered_map<std::string, double>& baseline,
    const std::unordered_map<std::string, EvaluationEntry>& opt) {

    std::cout << std::setw(20) << "Instance"
              << std::setw(12) << "Greedy"
              << std::setw(12) << "Baseline"
              << std::setw(12) << "Optimal"
              << std::setw(12) << "ΔBase"
              << std::setw(12) << "ΔOpt" << "\n";

    for (const auto& [name, gval] : greedy) {
        if (baseline.find(name) == baseline.end()) {
            std::cerr << "Error: Instance '" << name << "' not found in baseline values.\n";
            continue;
        }
        if (opt.find(name) == opt.end()) {
            std::cerr << "Error: Instance '" << name << "' not found in optimal values.\n";
            continue;
        }

        double bval = baseline.at(name);
        double oval = opt.at(name).value;
        bool is_opt = opt.at(name).is_optimal;

        double d_base = 100.0 * (bval - gval) / oval;
        double d_opt = 100.0 * (oval - gval) / oval;
        

        std::cout << std::setw(20) << name
                  << std::setw(12) << std::fixed << std::setprecision(2) << gval
                  << std::setw(12) << bval
                  << std::setw(12) << (is_opt ? std::to_string(oval) : std::to_string(oval) + "*")
                  << std::setw(12) << d_base
                  << std::setw(12) << d_opt << "\n";
    }
}

