cmake_minimum_required(VERSION 3.22)
project(cvrp_ga)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB source source/*.cpp)

add_executable(${PROJECT_NAME} ${source})