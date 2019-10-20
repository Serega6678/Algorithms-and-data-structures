#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <set>
#include "BinomialHeap.cpp"

TEST(BinomialHeap, insertGetMin1) {
    BinomialHeap heap;
    EXPECT_ANY_THROW(heap.getMin());
    heap.insert(5);
    EXPECT_EQ(heap.getMin(), 5);
    heap.insert(6);
    EXPECT_EQ(heap.getMin(), 5);
    heap.insert(4);
    EXPECT_EQ(heap.getMin(), 4);
    heap.insert(20);
    EXPECT_EQ(heap.getMin(), 4);
    heap.insert(1);
    EXPECT_EQ(heap.getMin(), 1);
}

TEST(BinomialHeap, InsertGetMin2) {
    int num = rand() % 10000;
    std::multiset<int> s;
    BinomialHeap heap;
    for (int i = 0; i < num; ++i) {
        int value = rand();
        s.insert(value);
        heap.insert(value);
        EXPECT_EQ(heap.getMin(), *s.begin());
    }
}

TEST(BinomialHeap, extractMin1) {
    BinomialHeap heap;
    EXPECT_ANY_THROW(heap.extractMin());
    heap.insert(3);
    heap.insert(5);
    heap.insert(4);
    EXPECT_EQ(heap.extractMin(), 3);
    EXPECT_EQ(heap.extractMin(), 4);
    heap.insert(3);
    EXPECT_EQ(heap.extractMin(), 3);
    EXPECT_EQ(heap.extractMin(), 5);
    EXPECT_ANY_THROW(heap.extractMin());
}

TEST(BinomialHeap, extractMin2andIsEmpty) {
    int num = rand() % 10000;
    std::multiset<int> s;
    BinomialHeap heap;
    ASSERT_EQ(heap.isEmpty(), s.empty());
    for (int i = 0; i < num; ++i) {
        int value = rand();
        s.insert(value);
        heap.insert(value);
        ASSERT_EQ(heap.isEmpty(), s.empty());
        EXPECT_EQ(heap.getMin(), *s.begin());
        if (rand() % 2) {
            EXPECT_EQ(heap.extractMin(), *s.begin());
            s.erase(s.begin());
        }
    }
}

TEST(BinomialHeap, ManipulationPointerInsert) {
    BinomialHeap heap;
    std::vector<int> values;
    std::vector<BinomialHeap::ManipulationPointer> ManipulationPointers;
    int num = rand() % 10000;
    for (int i = 0; i < num; ++i) {
        int value = rand();
        values.push_back(value);
        ManipulationPointers.push_back(heap.insert(value));
    }
    BinomialHeap::ManipulationPointer pointer;
    EXPECT_ANY_THROW(heap.getValue(pointer));
    for (int i = 0; i < num; ++i) {
        EXPECT_EQ(values[i],heap.getValue(ManipulationPointers[i]));
    }
}

TEST(BinomialHeap, merge) {
    BinomialHeap heap1;
    BinomialHeap heap2;
    std::vector<int> values;
    std::vector<BinomialHeap::ManipulationPointer> ManipulationPointers;
    int num = rand() % 10000;
    for (int i = 0; i < num; ++i) {
        int value = rand() % 100;
        values.push_back(value);
        if (rand()) {
            ManipulationPointers.push_back(heap1.insert(value));
        } else {
            ManipulationPointers.push_back(heap2.insert(value));
        }
    }
    heap1.merge(heap2);
    ASSERT_TRUE(heap2.isEmpty());
    for (int i = 0; i < values.size(); ++i) {
        EXPECT_EQ(heap1.getValue(ManipulationPointers[i]), values[i]);
    }
}

TEST(BinomialHeap, change1) {
    BinomialHeap heap;
    BinomialHeap::ManipulationPointer ptr2 = heap.insert(2);
    BinomialHeap::ManipulationPointer ptr1 = heap.insert(1);
    BinomialHeap::ManipulationPointer ptr3 = heap.insert(3);
    EXPECT_EQ(heap.getValue(ptr1), 1);
    EXPECT_EQ(heap.getValue(ptr2), 2);
    EXPECT_EQ(heap.getValue(ptr3), 3);
    heap.change(ptr2, 5);
    EXPECT_EQ(heap.getValue(ptr2), 5);
    EXPECT_EQ(heap.getMin(), 1);
    heap.change(ptr3, 0);
    EXPECT_EQ(heap.getValue(ptr3), 0);
    EXPECT_EQ(heap.getMin(), 0);
    heap.change(ptr3, 3);
    EXPECT_EQ(heap.getValue(ptr3), 3);
    EXPECT_EQ(heap.extractMin(), 1);
    EXPECT_ANY_THROW(heap.change(ptr1, 5));
    EXPECT_EQ(heap.getMin(), 3);
}

TEST(BinomialHeap, change2) {
    std::vector<int> values;
    std::vector<BinomialHeap::ManipulationPointer> ManipulationPointers;
    BinomialHeap heap;
    int num = rand() % 10000;
    for (int i = 0; i < num; ++i) {
        int value = rand();
        values.push_back(value);
        ManipulationPointers.push_back(heap.insert(value));
    }
    num = rand() % 10000;
    for (int i = 0; i < num; ++i) {
        size_t index = rand() % values.size();
        int value = rand();
        values[index] = value;
        heap.change(ManipulationPointers[index], value);
    }
    for (int i = 0; i < values.size(); ++i) {
        EXPECT_EQ(values[i], heap.getValue(ManipulationPointers[i]));
    }
}

TEST(BinomialHeap, erase1) {
    BinomialHeap heap;
    BinomialHeap::ManipulationPointer ptr1 = heap.insert(1);
    BinomialHeap::ManipulationPointer ptr2 = heap.insert(2);
    BinomialHeap::ManipulationPointer ptr3 = heap.insert(3);
    EXPECT_EQ(heap.getMin(), 1);
    heap.change(ptr2, 0);
    EXPECT_EQ(heap.getMin(), 0);
    heap.erase(ptr2);
    EXPECT_EQ(heap.getMin(), 1);
    heap.erase(ptr1);
    EXPECT_EQ(heap.getMin(), 3);
    EXPECT_ANY_THROW(heap.erase(ptr1));
}

TEST(BinomialHeap, erase2) {
    std::vector<int> values;
    std::vector<BinomialHeap::ManipulationPointer> ManipulationPointers;
    BinomialHeap heap;
    int num = rand() % 10000; 
    for (int i = 0; i < num; ++i) {
        int value = rand();
        values.push_back(value);
        ManipulationPointers.push_back(heap.insert(value));
    }
    num = rand() % values.size() - 1;
    for (int i = 0; i < num; ++i) {
        size_t index = rand() % values.size();
        heap.erase(ManipulationPointers[index]);
        values.erase(values.begin() + index);
        ManipulationPointers.erase(ManipulationPointers.begin() + index);
    }
    for (int i = 0; i < values.size(); ++i) {
        EXPECT_EQ(values[i], heap.getValue(ManipulationPointers[i]));
    }
}