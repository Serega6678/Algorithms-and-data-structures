cmake_minimum_required(VERSION 2.8)
project(BinomialHeap)

set(CMAKE_CXX_STANDARD 14)

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(BinomialHeap main.cpp BinomialHeap.cpp tests.cpp)

target_link_libraries(BinomialHeap ${GTEST_LIBRARIES} pthread)
