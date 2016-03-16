cmake_minimum_required(VERSION 3.3)
project(zad1 C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(SOURCE_FILES main.c)
add_executable(zad1 ${SOURCE_FILES})