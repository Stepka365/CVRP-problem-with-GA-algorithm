#include "genetic_solver.h"

#include <numeric>
#include <algorithm>

std::pair<GeneticSolver::chromosome_t, double> GeneticSolver::solve() {
    std::vector<size_t> clients(m_n_clients);
    std::iota(clients.begin(), clients.end(), 1); // Заполняем 1, 2, ..., N

    std::vector<chromosome_t> population(m_population_size, clients);
    for (auto& ind : population)
        std::shuffle(ind.begin(), ind.end(), m_gen);

    for (size_t g = 0; g < m_n_gen; g++) {
        // Сортировка по дистанции
        std::sort(population.begin(),
                  population.end(),
                  [&](const chromosome_t& a, const chromosome_t& b) {
                      return compute_distance(a) < compute_distance(b);
                  });

        std::vector<chromosome_t> next_gen;
        next_gen.push_back(population[0]); // Элитизм
        next_gen.push_back(population[1]);

        while (next_gen.size() < m_population_size) {
            size_t i1 = std::rand() % (m_population_size / 2); // Турнирный отбор из лучших
            size_t i2 = std::rand() % (m_population_size / 2);
            chromosome_t child = crossover(population[i1], population[i2]);
            mutate(child);
            next_gen.push_back(child);
        }
        population = next_gen;
    }
    return {population[0], compute_distance(population[0])};
}

double GeneticSolver::compute_distance(const chromosome_t& chromosome) const {
    double total_dist = 0;
    size_t current_load = 0;
    size_t curr_node = 0; // Депо

    for (size_t next_node : chromosome) {
        if (current_load + m_demands[next_node] <= m_capacity) {
            total_dist += distance(m_coords[curr_node], m_coords[next_node]);
            current_load += m_demands[next_node];
            curr_node = next_node;
        }
        else {
            // Возврат в депо и выезд к новому клиенту
            total_dist += m_coords[curr_node].length() + m_coords[next_node].length();
            current_load = m_demands[next_node];
            curr_node = next_node;
        }
    }
    total_dist += m_coords[curr_node].length(); // Финальный возврат
    return total_dist;
}

GeneticSolver::chromosome_t GeneticSolver::crossover(const chromosome_t& parent1, const chromosome_t& parent2) const {
    chromosome_t child(m_n_clients, -1);
    size_t a = std::rand() % m_n_clients;
    size_t b = std::rand() % m_n_clients;
    if (a > b) std::swap(a, b);

    for (size_t i = a; i <= b; i++) child[i] = parent1[i];

    int p2_idx = 0;
    for (size_t i = 0; i < m_n_clients; ++i) {
        if (child[i] == -1) {
            while (std::find(child.begin(), child.end(), parent2[p2_idx]) != child.end())
                p2_idx++;
            child[i] = parent2[p2_idx];
        }
    }
    return child;
}

void GeneticSolver::mutate(chromosome_t& chromosome) const {
    if ((double) std::rand() / RAND_MAX > m_mutation_rate) return;

    size_t i = std::rand() % m_n_clients;
    size_t j = std::rand() % m_n_clients;
    std::swap(chromosome[i], chromosome[j]);
}
