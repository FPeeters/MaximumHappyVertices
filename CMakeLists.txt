cmake_minimum_required(VERSION 3.15)
project(MaxHappyVerts)

set(CMAKE_CXX_STANDARD 14)

add_executable(main main.cpp Graph.cpp Graph.h ConstructiveAlgs.cpp ConstructiveAlgs.h SimulatedAnnealing.cpp SimulatedAnnealing.h)