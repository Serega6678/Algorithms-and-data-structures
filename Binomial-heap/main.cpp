#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "BinomialHeap.cpp"


int main(int argr, char* argv[]) {
    testing::InitGoogleTest(&argr, argv);
    return  RUN_ALL_TESTS();
}