cmake_minimum_required(VERSION 3.17)
project(oving4)

# To disable runtime checks, remove "-fsanitize=address" below

set(CMAKE_CXX_STANDARD 14)

add_executable(oving4 main.cpp Workers.cpp Workers.h)