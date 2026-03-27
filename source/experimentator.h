#pragma once

#include <vector>

class experimentator {
public:
    void run_all_experiments();
private:
    void print_results(const std::vector<size_t>&, double);
    void exp1();
    void exp2();
};
