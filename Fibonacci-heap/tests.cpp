#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <set>
#include <vector>
#include "FibonacciHeap.cpp"

TEST(FibonacciHeap, insertGetMin1) {
    FibonacciHeap heap;
    EXPECT_ANY_THROW(heap.getMin());
    heap.insert(3);
    EXPECT_EQ(heap.getMin(), 3);
    heap.insert(4);
    EXPECT_EQ(heap.getMin(), 3);
    heap.insert(5);
    EXPECT_EQ(heap.getMin(), 3);
    heap.insert(2);
    EXPECT_EQ(heap.getMin(), 2);
    heap.insert(6);
    EXPECT_EQ(heap.getMin(), 2);
    heap.insert(1);
    EXPECT_EQ(heap.getMin(), 1);
    heap.insert(20);
    EXPECT_EQ(heap.getMin(), 1);
}

TEST(FibonacciHeap, insertGetMin2) {
    FibonacciHeap heap;
    EXPECT_ANY_THROW(heap.getMin());
    std::multiset<int> s;
    int num = rand() % 10000;
    ASSERT_TRUE(heap.isEmpty());
    for (int i = 0; i < num; ++i) {
        int value = rand();
        s.insert(value);
        heap.insert(value);
        EXPECT_EQ(*s.begin(), heap.getMin());
    }
}

TEST(FibonacciHeap, extractMin1) {
    FibonacciHeap heap;
    EXPECT_ANY_THROW(heap.extractMin());
    heap.insert(3);
    EXPECT_EQ(heap.getMin(), 3);
    heap.insert(4);
    EXPECT_EQ(heap.getMin(), 3);
    heap.extractMin();
    EXPECT_EQ(heap.getMin(), 4);
    heap.insert(5);
    EXPECT_EQ(heap.getMin(), 4);
    heap.extractMin();
    EXPECT_EQ(heap.getMin(), 5);
    heap.insert(2);
    EXPECT_EQ(heap.getMin(), 2);
    heap.insert(6);
    EXPECT_EQ(heap.getMin(), 2);
    heap.extractMin();
    EXPECT_EQ(heap.getMin(), 5);
    heap.insert(1);
    EXPECT_EQ(heap.getMin(), 1);
    heap.insert(20);
    EXPECT_EQ(heap.getMin(), 1);
    heap.extractMin();
    EXPECT_EQ(heap.getMin(), 5);
    heap.extractMin();
    EXPECT_EQ(heap.getMin(), 6);
    heap.extractMin();
    EXPECT_EQ(heap.extractMin(), 20);
    EXPECT_ANY_THROW(heap.extractMin());
}

TEST(FibonacciHeap, extractMin2) {
    FibonacciHeap heap;
    std::multiset<int> s;
    int num = rand() % 10000;
    ASSERT_TRUE(heap.isEmpty());
    for (int i = 0; i < num; ++i) {
        int value = rand();
        s.insert(value);
        heap.insert(value);
        EXPECT_EQ(*s.begin(), heap.getMin());
        if (rand() % 2) {
            s.erase(s.begin()) ;
            heap.extractMin();
        }
    }
}

TEST(FibonacciHeap, isEmpty) {
    FibonacciHeap heap;
    ASSERT_TRUE(heap.isEmpty());
    heap.insert(0);
    ASSERT_TRUE(!heap.isEmpty());
    heap.insert(5);
    ASSERT_TRUE(!heap.isEmpty());
    heap.extractMin();
    ASSERT_TRUE(!heap.isEmpty());
    heap.extractMin();
    ASSERT_TRUE(heap.isEmpty());
    heap.insert(2);
    ASSERT_TRUE(!heap.isEmpty());
    heap.extractMin();
    ASSERT_TRUE(heap.isEmpty());
}

TEST(FibonacciHeap, decreaseKey1) {
    FibonacciHeap heap;
    heap.insert(1);
    heap.insert(3);
    FibonacciHeap::ManipulationPointer ptr = heap.insert(4);
    EXPECT_EQ(heap.extractMin(), 1);
    heap.decreaseKey(ptr, 2);
    EXPECT_EQ(heap.extractMin(), 2);
    EXPECT_ANY_THROW(heap.decreaseKey(ptr, 100));
    EXPECT_ANY_THROW(heap.decreaseKey(ptr, 1));
    EXPECT_EQ(heap.extractMin(), 3);
    EXPECT_ANY_THROW(heap.decreaseKey(ptr, 1));
}

TEST(FibonacciHeap, decreaseKey2) {
    FibonacciHeap heap;
    heap.insert(2);
    FibonacciHeap::ManipulationPointer ptr3 = heap.insert(3);
    FibonacciHeap::ManipulationPointer ptr4 = heap.insert(4);
    EXPECT_EQ(heap.getMin(), 2);
    heap.decreaseKey(ptr4, 0);
    EXPECT_EQ(heap.getMin(), 0);
    EXPECT_EQ(heap.extractMin(), 0);
    EXPECT_EQ(heap.extractMin(), 2);
    EXPECT_EQ(heap.getMin(), 3);
    heap.decreaseKey(ptr3, -1);
    EXPECT_EQ(heap.getMin(), -1);
    heap.decreaseKey(ptr3, -10);
    EXPECT_EQ(heap.extractMin(), -10);
}

TEST(FibonacciHeap, merge) {
    FibonacciHeap heap1, heap2;
    FibonacciHeap::ManipulationPointer ptr0 = heap1.insert(0);
    FibonacciHeap::ManipulationPointer ptr1 = heap1.insert(1);
    FibonacciHeap::ManipulationPointer ptr2= heap2.insert(2);
    FibonacciHeap::ManipulationPointer ptr3 = heap2.insert(3);
    FibonacciHeap::ManipulationPointer ptr4 = heap1.insert(4);
    heap1.merge(heap2);
    ASSERT_TRUE(heap2.isEmpty());
    EXPECT_EQ(heap1.getValue(ptr0), 0);
    EXPECT_EQ(heap1.getValue(ptr1), 1);
    EXPECT_EQ(heap1.getValue(ptr2), 2);
    EXPECT_EQ(heap1.getValue(ptr3), 3);
    EXPECT_EQ(heap1.getValue(ptr4), 4);
    EXPECT_EQ(heap1.getMin(), 0);
    heap1.decreaseKey(ptr3, -5);
    EXPECT_EQ(heap1.getMin(), -5);
    EXPECT_EQ(heap1.getValue(ptr3), -5);
}
