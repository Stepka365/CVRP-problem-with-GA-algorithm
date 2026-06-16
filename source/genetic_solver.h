#pragma once

#include "point.h"

#include <vector>
#include <random>


class GeneticSolver {
public:
    using chromosome_t = std::vector<size_t>;

    GeneticSolver(std::vector<Point> coords,
                  std::vector<size_t> demands,
                  size_t capacity);

    void set_population_size(size_t population_size) { m_population_size = population_size; }
    void set_n_gen(size_t n_gen) { m_n_gen = n_gen; }
    void set_mutation_rate(double mutation_rate) { m_mutation_rate = mutation_rate; }

    std::pair<chromosome_t, double> solve();
private:
    struct PopulationMember {
        PopulationMember() = default;
        PopulationMember(size_t clients);
        chromosome_t chromosome;
        double distance = 0.0;
    };
    static size_t tournament_select(const std::vector<PopulationMember>& population, size_t pop_size, std::mt19937& gen);
    double compute_distance(chromosome_t& chromosome);
    chromosome_t crossover(const chromosome_t& parent1, const chromosome_t& parent2) const;
    void mutate(chromosome_t& chromosome, double rate) const;
    void two_opt(std::vector<size_t>& route, size_t start, size_t end) const;
    std::vector<Point> m_coords;
    std::vector<size_t> m_demands;
    size_t m_capacity = 0;
    size_t m_n_clients;
    size_t m_population_size = 50;
    size_t m_n_gen = 200;
    double m_mutation_rate = 0.1;

    inline static std::mt19937 m_gen{std::random_device{}()};
};
