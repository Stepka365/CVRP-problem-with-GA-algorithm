#pragma once

#include <cmath>

class Point {
public:
    Point() = default;
    Point(int x, int y) : x(x), y(y) {}
    double length() const { return std::hypot(x, y); }
public:
    int x = 0.0;
    int y = 0.0;
};

inline double distance(const Point& p1, const Point& p2) {
    return std::hypot(p1.x - p2.x, p1.y - p2.y);
}