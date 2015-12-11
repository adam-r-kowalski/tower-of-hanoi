cmake_minimum_required(VERSION 3.3)
project(tower-of-hanoi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES toh.cpp)
add_executable(toh ${SOURCE_FILES})
