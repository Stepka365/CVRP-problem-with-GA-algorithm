#include "experimentator.h"

#include "genetic_solver.h"

#include <ctime>
#include <iostream>
#include <iomanip>

void experimentator::run_all_experiments() {
    std::srand(std::time(nullptr));
    exp1();
    exp2();
}

void experimentator::print_results(const std::vector<size_t>& best, double distance) {
    std::cout << "Best distance: " << std::fixed << std::setprecision(2) << distance << std::endl;
    std::cout << "Route: ";
    for (size_t n : best) std::cout << n << " ";
    std::cout << std::endl << std::endl;
}

void experimentator::exp1() {
    std::vector<Point> coords = {{0, 0}, {1, 2}, {3, 1}, {4, 4}, {-1, 3}, {2, -2}};
    std::vector<size_t> demands = {0, 2, 3, 2, 1, 4}; // Спрос (0 для депо)
    size_t capacity = 5;

    GeneticSolver task(std::move(coords), std::move(demands), capacity);
    auto [best, distance] = task.solve();
    print_results(best, distance);
}

void experimentator::exp2() {
    // --- Данные для эксперимента №2 (10 клиентов) ---
    // Индекс 0: Депо (центр)
    // Индексы 1-10: Клиенты в разных квадрантах
    std::vector<Point> coords = {
        {0, 0},
        // 0: Депо
        {10, 10},
        {12, 8},
        {15, 12},
        // Группа 1 (Северо-восток)
        {-10, 10},
        {-12, 12},
        // Группа 2 (Северо-запад)
        {-10, -10},
        {-15, -12},
        // Группа 3 (Юго-запад)
        {10, -10},
        {12, -8},
        {8, -12} // Группа 4 (Юго-восток)
    };

    // Спрос клиентов (суммарно 25 единиц)
    std::vector<size_t> demands = {0, 2, 4, 3, 2, 3, 2, 4, 1, 2, 2};

    // Вместимость автомобиля (потребуется минимум 3-4 рейса)
    size_t capacity = 8;

    GeneticSolver task(std::move(coords), std::move(demands), capacity);
    task.set_population_size(100);
    task.set_n_gen(1000);
    task.set_mutation_rate(0.1);
    auto [best, distance] = task.solve();
    print_results(best, distance);
}
