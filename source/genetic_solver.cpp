#include "genetic_solver.h"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <cmath>

static double pt_dist(const Point& a, const Point& b) {
    return std::hypot(a.x - b.x, a.y - b.y);
}

GeneticSolver::GeneticSolver(std::vector<Point> coords, std::vector<size_t> demands, size_t capacity):
    m_coords(std::move(coords)),
    m_demands(std::move(demands)),
    m_capacity(capacity),
    m_n_clients(m_demands.size() - 1) {
}

GeneticSolver::PopulationMember::PopulationMember(size_t clients) : chromosome(clients) {
    std::iota(chromosome.begin(), chromosome.end(), 1);
}

std::pair<GeneticSolver::chromosome_t, double> GeneticSolver::solve() {
    std::vector<PopulationMember> population(m_population_size, m_n_clients);
    for (auto& member : population) {
        std::shuffle(member.chromosome.begin(), member.chromosome.end(), m_gen);
        member.distance = compute_distance(member.chromosome);
    }

    for (size_t cur_generation = 0; cur_generation < m_n_gen; cur_generation++) {
        double t = static_cast<double>(cur_generation) / m_n_gen;
        double gen_mutation_rate = m_mutation_rate * (1.0 - t) + 0.02 * t;

        std::sort(population.begin(), population.end(),
                  [](const PopulationMember& a, const PopulationMember& b) {
                      return a.distance < b.distance;
                  });

        std::vector<PopulationMember> next_gen;
        next_gen.reserve(m_population_size);
        next_gen.push_back(population[0]);
        next_gen.push_back(population[1]);

        while (next_gen.size() < m_population_size) {
            size_t i1 = tournament_select(population, m_population_size, m_gen);
            size_t i2 = tournament_select(population, m_population_size, m_gen);
            PopulationMember child;
            child.chromosome = crossover(population[i1].chromosome, population[i2].chromosome);
            mutate(child.chromosome, gen_mutation_rate);
            child.distance = compute_distance(child.chromosome);
            next_gen.push_back(std::move(child));
        }
        population = std::move(next_gen);
    }
    return {population[0].chromosome, population[0].distance};
}

size_t GeneticSolver::tournament_select(const std::vector<PopulationMember>& population, size_t pop_size, std::mt19937& gen) {
    std::uniform_int_distribution<size_t> dist_idx(0, pop_size - 1);
    size_t best = dist_idx(gen);
    for (int k = 1; k < 3; ++k) {
        size_t idx = dist_idx(gen);
        if (population[idx].distance < population[best].distance)
            best = idx;
    }
    return best;
}

static bool try_two_opt(std::vector<size_t>& route, size_t start, size_t end,
                        const std::vector<Point>& coords) {
    for (size_t i = start; i <= end; ++i) {
        for (size_t j = i + 1; j <= end; ++j) {
            double d_i_1_i = pt_dist(coords[route[i-1]], coords[route[i]]);
            double d_j_j_1 = pt_dist(coords[route[j]],   coords[route[j+1]]);
            double d_i_1_j = pt_dist(coords[route[i-1]], coords[route[j]]);
            double d_i_j_1 = pt_dist(coords[route[i]],   coords[route[j+1]]);
            double delta = -d_i_1_i - d_j_j_1 + d_i_1_j + d_i_j_1;
            if (delta < -1e-10) {
                std::reverse(route.begin() + i, route.begin() + j + 1);
                return true;
            }
        }
    }
    return false;
}

void GeneticSolver::two_opt(std::vector<size_t>& route, size_t start, size_t end) const {
    if (start >= end) return;
    while (try_two_opt(route, start, end, m_coords));
}

double GeneticSolver::compute_distance(chromosome_t& chromosome) {
    std::vector<size_t> full_route;
    full_route.reserve(chromosome.size() + m_n_clients);
    full_route.push_back(0);
    size_t current_load = 0;

    for (size_t next_node : chromosome) {
        if (current_load + m_demands[next_node] <= m_capacity) {
            full_route.push_back(next_node);
            current_load += m_demands[next_node];
        } else {
            full_route.push_back(0);
            full_route.push_back(next_node);
            current_load = m_demands[next_node];
        }
    }
    full_route.push_back(0);

    size_t seg_start = 1;
    for (size_t i = 1; i < full_route.size() - 1; ++i) {
        if (full_route[i] == 0) {
            two_opt(full_route, seg_start, i - 1);
            seg_start = i + 1;
        }
    }
    if (seg_start < full_route.size() - 1) {
        two_opt(full_route, seg_start, full_route.size() - 2);
    }

    chromosome.clear();
    std::copy_if(full_route.begin(), full_route.end(), std::back_inserter(chromosome),
        [](size_t node) { return node != 0; });

    double total_dist = 0;
    for (size_t i = 1; i < full_route.size(); ++i) {
        total_dist += pt_dist(m_coords[full_route[i-1]], m_coords[full_route[i]]);
    }
    return total_dist;
}

GeneticSolver::chromosome_t GeneticSolver::crossover(const chromosome_t& parent1, const chromosome_t& parent2) const {
    std::uniform_int_distribution<size_t> dist(0, m_n_clients - 1);
    chromosome_t child(m_n_clients, -1);
    auto [a, b] = std::minmax(dist(m_gen), dist(m_gen));

    auto p2_it = parent2.begin();
    for (size_t i = 0; i < child.size(); ++i) {
        if (a <= i && i < b) {
            child[i] = parent1[i];
        } else {
            while (std::find(parent1.begin() + a, parent1.begin() + b, *p2_it) != parent1.begin() + b)
                ++p2_it;
            child[i] = *p2_it;
            ++p2_it;
        }
    }
    return child;
}

void GeneticSolver::mutate(chromosome_t& chromosome, double rate) const {
    std::uniform_real_distribution<double> dist_real(0.0, 1.0);
    if (dist_real(m_gen) > rate)
        return;

    std::uniform_int_distribution<size_t> dist_idx(0, m_n_clients - 1);
    std::swap(chromosome[dist_idx(m_gen)], chromosome[dist_idx(m_gen)]);
}
