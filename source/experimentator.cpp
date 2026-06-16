#include "experimentator.h"

#include "genetic_solver.h"
#include "parser.h"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <map>
#include <filesystem>

static const std::map<std::string, double> g_optima = {
    {"A-n32-k5", 784}, {"A-n33-k5", 661}, {"A-n33-k6", 742},
    {"A-n34-k5", 778}, {"A-n36-k5", 799}, {"A-n37-k5", 669},
    {"A-n37-k6", 949}, {"A-n38-k5", 730}, {"A-n39-k5", 822},
    {"A-n39-k6", 831}, {"A-n44-k6", 937}, {"A-n45-k6", 944},
    {"A-n45-k7", 1146}, {"A-n46-k7", 914}, {"A-n48-k7", 1073},
    {"A-n53-k7", 1010}, {"A-n54-k7", 1167}, {"A-n55-k9", 1073},
    {"A-n60-k9", 1354}, {"A-n61-k9", 1034}, {"A-n62-k8", 1288},
    {"A-n63-k9", 1616}, {"A-n63-k10", 1314}, {"A-n64-k9", 1401},
    {"A-n65-k9", 1174}, {"A-n69-k9", 1159}, {"A-n80-k10", 1763},
    {"B-n31-k5", 672}, {"B-n34-k5", 788}, {"B-n35-k5", 955},
    {"B-n38-k6", 805}, {"B-n39-k5", 549}, {"B-n41-k6", 829},
    {"B-n43-k6", 742}, {"B-n44-k7", 909}, {"B-n45-k5", 751},
    {"B-n45-k6", 678}, {"B-n50-k7", 741}, {"B-n50-k8", 1312},
    {"B-n51-k7", 1032}, {"B-n52-k7", 747}, {"B-n56-k7", 707},
    {"B-n57-k7", 1153}, {"B-n57-k9", 1598}, {"B-n63-k10", 1496},
    {"B-n64-k9", 861}, {"B-n66-k9", 1316}, {"B-n67-k10", 1032},
    {"B-n68-k9", 1272}, {"B-n78-k10", 1221},
    {"E-n22-k4", 375}, {"E-n23-k3", 569},
    {"E-n30-k3", 534}, {"E-n33-k4", 835},
    {"E-n51-k5", 521}, {"E-n76-k7", 682}, {"E-n76-k8", 735},
    {"E-n76-k10", 830}, {"E-n76-k14", 1021}, {"E-n101-k8", 815},
    {"E-n101-k14", 1067}
};

static std::string stem(const std::string& filename) {
    auto start = filename.find_last_of("/\\");
    auto end = filename.rfind('.');
    if (start == std::string::npos) start = 0; else ++start;
    if (end == std::string::npos) end = filename.size();
    return filename.substr(start, end - start);
}

void experimentator::run_all_experiments() {
    const auto& optima = g_optima;
    constexpr double error_threshold = 0.10;
    constexpr size_t max_trials = 10;

    std::cout << std::left << std::setw(14) << "Instance"
              << std::right << std::setw(8) << "Nodes"
              << std::setw(10) << "Optimum"
              << std::setw(12) << "Best"
              << std::setw(10) << "Error%"
              << std::setw(12) << "Time(ms)"
              << std::setw(10) << "Trials"
              << "  Status" << std::endl;
    std::cout << std::string(78, '-') << std::endl;

    size_t passed = 0, total = 0;

    for (const auto& [name, known_optimum] : optima) {
        std::string filename = name + ".vrp";
        std::string filepath = filename;

        CVRPData original_data;
        try {
            original_data = CVRPParser::parse(filepath);
        } catch (...) {
            std::cout << std::left << std::setw(14) << name
                      << "  FILE NOT FOUND" << std::endl;
            continue;
        }

        ++total;
        double best_distance = 1e18;
        long long total_time = 0;
        size_t trials_used = 0;

        for (size_t trial = 1; trial <= max_trials; ++trial) {
            CVRPData data = original_data;
            GeneticSolver task(std::move(data.coords), std::move(data.demands), data.capacity);
            task.set_population_size(300);
            task.set_n_gen(1000);
            task.set_mutation_rate(0.1);

            auto start = std::chrono::steady_clock::now();
            auto [chrom, distance] = task.solve();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start).count();

            total_time += elapsed;
            trials_used = trial;

            if (distance < best_distance)
                best_distance = distance;

            double error = (distance - known_optimum) / known_optimum;
            if (error <= error_threshold)
                break;
        }

        double final_error = (best_distance - known_optimum) / known_optimum;
        std::string status = (final_error <= error_threshold) ? "OK" : "FAIL";

        std::cout << std::left << std::setw(14) << name
                  << std::right << std::setw(8) << original_data.dimension
                  << std::setw(10) << std::fixed << std::setprecision(0) << known_optimum
                  << std::setw(12) << std::fixed << std::setprecision(2) << best_distance
                  << std::setw(10) << std::fixed << std::setprecision(2) << (final_error * 100)
                  << std::setw(12) << total_time
                  << std::setw(10) << trials_used
                  << "  " << status << std::endl;

        if (status == "OK") ++passed;
    }

    std::cout << std::string(78, '-') << std::endl;
    std::cout << "Passed: " << passed << " / " << total
              << " (" << std::fixed << std::setprecision(1)
              << (static_cast<double>(passed) / total * 100) << "%)" << std::endl;
}
