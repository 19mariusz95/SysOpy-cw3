cmake_minimum_required(VERSION 3.3)
project(zad2 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_executable(fcounter1 main.c)
add_executable(fcounter2 main2.c)
add_executable(fcounter3 main3.c)