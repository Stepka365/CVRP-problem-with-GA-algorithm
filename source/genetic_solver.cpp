#include "genetic_solver.h"

#include <numeric>
#include <algorithm>

GeneticSolver::GeneticSolver(std::vector<Point>&& coords, std::vector<size_t>&& demands, size_t capacity):
    m_coords(std::move(coords)),
    m_demands(std::move(demands)),
    m_capacity(capacity) {
    std::iota(random_chromosome.begin(), random_chromosome.end(), 1); // Заполняем 1, 2, ..., N
}

std::pair<GeneticSolver::chromosome_t, double> GeneticSolver::solve() {
    std::vector<PopulationMember> population(m_population_size, m_n_clients);
    for (auto& member : population) {
        std::shuffle(member.chromosome.begin(), member.chromosome.end(), m_gen);
        member.distance = compute_distance(member.chromosome);
    }

    for (size_t g = 0; g < m_n_gen; g++) {
        // Сортировка по дистанции
        std::sort(population.begin(),
                  population.end(),
                  [](const PopulationMember& a, const PopulationMember& b) {
                      return a.distance < b.distance;
                  });

        std::vector<PopulationMember> next_gen;
        next_gen.reserve(m_population_size);
        next_gen.push_back(population[0]); // Элитизм
        next_gen.push_back(population[1]);

        while (next_gen.size() < m_population_size) {
            size_t i1 = std::rand() % (m_population_size / 2); // Турнирный отбор из лучших
            size_t i2 = std::rand() % (m_population_size / 2);
            PopulationMember child;
            child.chromosome = crossover(population[i1].chromosome, population[i2].chromosome);
            mutate(child.chromosome);
            child.distance = compute_distance(child.chromosome);
            next_gen.push_back(std::move(child));
        }
        population = std::move(next_gen);
    }
    return {population[0].chromosome, population[0].distance};
}

double GeneticSolver::compute_distance(const chromosome_t& chromosome) const {
    double total_dist = 0;
    size_t current_load = 0;
    size_t cur_node = 0; // Депо

    for (size_t next_node : chromosome) {
        if (current_load + m_demands[next_node] <= m_capacity) {
            total_dist += distance(m_coords[cur_node], m_coords[next_node]);
            current_load += m_demands[next_node];
            cur_node = next_node;
        }
        else {
            // Возврат в депо и выезд к новому клиенту
            total_dist += distance(m_coords[cur_node], m_coords[0]) + distance(m_coords[0], m_coords[next_node]);
            current_load = m_demands[next_node];
            cur_node = next_node;
        }
    }

    total_dist += distance(m_coords[cur_node], m_coords[0]); // Финальный возврат
    return total_dist;
}

GeneticSolver::chromosome_t GeneticSolver::crossover(const chromosome_t& parent1, const chromosome_t& parent2) const {
    chromosome_t child(m_n_clients, -1);
    auto [a, b] = std::minmax(std::rand() % m_n_clients, std::rand() % m_n_clients);

    auto p2_cur_it = parent2.begin();
    for (size_t i = 0; i < child.size(); ++i) {
        if (a <= i && i < b) child[i] = parent1[i];
        else {
            while (std::find(parent1.begin() + a, parent1.begin() + b, *p2_cur_it) != parent1.begin() + b)
                ++p2_cur_it;
            child[i] = *p2_cur_it;
            ++p2_cur_it;
        }
    }
    return child;
}

void GeneticSolver::mutate(chromosome_t& chromosome) const {
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    if (distribution(m_gen) > m_mutation_rate)
        return;

    size_t i = std::rand() % m_n_clients;
    size_t j = std::rand() % m_n_clients;
    std::swap(chromosome[i], chromosome[j]);
}
