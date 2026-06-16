#include "parser.h"

#include <fstream>
#include <sstream>

static size_t string_to_ui(const std::string& string) {
    std::stringstream sstr(string);
    size_t number;
    sstr >> number;
    return number;
}

CVRPData CVRPParser::parse(const std::string& filename) {
    std::ifstream file(filename);
    CVRPData data;
    std::string line;

    if (!file.is_open())
        throw std::runtime_error("Unable to open file");

    // 1. Парсим заголовок
    while (std::getline(file, line) && line.find("NODE_COORD_SECTION") == std::string::npos) {
        if (line.find("DIMENSION") != std::string::npos)
            data.dimension = string_to_ui(line.substr(line.find(":") + 1));
        else if (line.find("CAPACITY") != std::string::npos)
            data.capacity = string_to_ui(line.substr(line.find(":") + 1));
    }

    data.coords.resize(data.dimension);
    data.demands.resize(data.dimension);

    // 2. Парсим координаты (NODE_COORD_SECTION)
    size_t skip_id;
    for (auto& point: data.coords)
        file >> skip_id >> point.x >> point.y;

    // 3. Ищем секцию спроса (DEMAND_SECTION)
    while (std::getline(file, line) && line.find("DEMAND_SECTION") == std::string::npos);

    // 4. Парсим спрос
    for (auto& demand: data.demands)
        file >> skip_id >> demand;

    return data;
}
