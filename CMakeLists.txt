cmake_minimum_required(VERSION 3.22)
project(cvrp_ga)

file(GLOB source source/*.cpp)

add_executable(${PROJECT_NAME} ${source})