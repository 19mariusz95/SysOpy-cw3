cmake_minimum_required(VERSION 3.3)
project(zad2 C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c)
add_executable(zad2 ${SOURCE_FILES})