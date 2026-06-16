#pragma once

#include "point.h"

#include <string>
#include <vector>

struct CVRPData {
    size_t dimension = 0;
    size_t capacity = 0;
    std::vector<Point> coords;
    std::vector<size_t> demands;
};

class CVRPParser {
public:
    static CVRPData parse(const std::string& filename);
};

