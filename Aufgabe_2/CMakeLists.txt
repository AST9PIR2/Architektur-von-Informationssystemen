cmake_minimum_required(VERSION 3.22)
project(Aufgabe_2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -pthread")

include_directories(.)

add_executable(Aufgabe_2
        main.cpp
        smpi.h
        smpi.cpp)
