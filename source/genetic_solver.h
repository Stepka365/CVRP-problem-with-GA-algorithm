#pragma once

#include "point.h"

#include <vector>
#include <random>

class GeneticSolver {
public:
    using chromosome_t = std::vector<size_t>;

    GeneticSolver(std::vector<Point>&& coords,
                  std::vector<size_t>&& demands,
                  size_t capacity) :
        m_coords(std::move(coords)),
        m_demands(std::move(demands)),
        m_capacity(capacity) {}

    void set_population_size(size_t population_size) { m_population_size = population_size; }
    void set_n_gen(size_t n_gen) { m_n_gen = n_gen; }
    void set_mutation_rate(double mutation_rate) { m_mutation_rate = mutation_rate; }
    std::pair<chromosome_t, double> solve();
private:
    double compute_distance(const chromosome_t& chromosome) const;
    chromosome_t crossover(const chromosome_t& chromosome1, const chromosome_t& chromosome2) const;
    void mutate(chromosome_t& chromosome) const;
private:
    std::vector<Point> m_coords;
    std::vector<size_t> m_demands;
    size_t m_capacity = 0.0;
    size_t m_n_clients = m_demands.size() - 1;
    size_t m_population_size = 50;
    size_t m_n_gen = 200;
    double m_mutation_rate = 0.1;

    std::mt19937 m_gen{std::random_device{}()};
};
