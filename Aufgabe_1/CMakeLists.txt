cmake_minimum_required(VERSION 3.22)
project(Aufgabe_1 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(Aufgabe_1 jacobi_algorithm.c)


