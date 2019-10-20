cmake_minimum_required(VERSION 2.8)
project(FibonacciHeap)

set(CMAKE_CXX_STANDARD 14)

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(FibonacciHeap main.cpp FibonacciHeap.cpp tests.cpp)

target_link_libraries(FibonacciHeap ${GTEST_LIBRARIES} pthread)
