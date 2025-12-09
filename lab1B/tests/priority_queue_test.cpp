#include "priority_queue.h"
#include <gtest/gtest.h>
#include <climits>
#include <vector>
#include <sstream>
#include <stdexcept>

// Тест: конструктор по умолчанию создаёт max-heap и пустую очередь
TEST(PriorityQueueTest, DefaultConstructor) {
    PriorityQueue pq;
    EXPECT_TRUE(pq.is_max_heap());
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0);
}

// Тест: конструктор с флагом позволяет выбрать max/min heap
TEST(PriorityQueueTest, ConstructorWithFlag) {
    PriorityQueue max_pq(true);
    PriorityQueue min_pq(false);
    EXPECT_TRUE(max_pq.is_max_heap());
    EXPECT_FALSE(min_pq.is_max_heap());
}

// Тест: конструктор из вектора строит корректную кучу за O(n)
TEST(PriorityQueueTest, ConstructorFromVector) {
    std::vector<int> data = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    PriorityQueue pq(data, true);
    EXPECT_EQ(pq.size(), data.size());
    EXPECT_EQ(pq.top(), 16); 

    PriorityQueue pq_min(data, false);
    EXPECT_EQ(pq_min.top(), 1);
}

// Тест: push в max-heap поддерживает порядок
TEST(PriorityQueueTest, PushMaxHeap) {
    PriorityQueue pq(true);
    pq.push(1);
    pq.push(3);
    pq.push(2);
    EXPECT_EQ(pq.top(), 3);
    pq.push(5);
    EXPECT_EQ(pq.top(), 5);
}

// Тест: push в min-heap поддерживает порядок
TEST(PriorityQueueTest, PushMinHeap) {
    PriorityQueue pq(false);
    pq.push(3);
    pq.push(1);
    pq.push(2);
    EXPECT_EQ(pq.top(), 1);
    pq.push(0);
    EXPECT_EQ(pq.top(), 0);
}

// Тест: pop в max-heap извлекает в порядке убывания
// Тест: pop в max-heap извлекает в порядке убывания
TEST(PriorityQueueTest, PopMaxHeap) {
  PriorityQueue pq(true);
  pq.push(1);
  pq.push(3);
  pq.push(2);

  EXPECT_EQ(pq.top(), 3);
  pq.pop();

  EXPECT_EQ(pq.top(), 2);
  pq.pop();

  EXPECT_EQ(pq.top(), 1);
  pq.pop();

  EXPECT_TRUE(pq.empty());
}

// Тест: top() и pop() на пустой очереди выбрасывают std::out_of_range (как в STL)
TEST(PriorityQueueTest, EmptyQueueThrows) {
    PriorityQueue pq;
    EXPECT_TRUE(pq.empty());

    EXPECT_THROW(pq.top(), std::out_of_range);

    EXPECT_THROW(pq.pop(), std::out_of_range);
}

// Тест: empty() и size() корректны в динамике
TEST(PriorityQueueTest, EmptyAndSize) {
    PriorityQueue pq;
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0u);

    pq.push(10);
    EXPECT_FALSE(pq.empty());
    EXPECT_EQ(pq.size(), 1u);

    pq.push(20);
    EXPECT_EQ(pq.size(), 2u);

    pq.pop();
    EXPECT_EQ(pq.size(), 1u);
    pq.pop();
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0u);
}

// Тест: clear() делает очередь пустой
TEST(PriorityQueueTest, Clear) {
    PriorityQueue pq;
    pq.push(1);
    pq.push(2);
    pq.push(3);
    EXPECT_EQ(pq.size(), 3u);

    pq.clear();
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0u);
}

// Тест: reserve() не ломает логику
TEST(PriorityQueueTest, Reserve) {
    PriorityQueue pq;
    pq.reserve(1000);
    for (int i = 0; i < 100; ++i) {
        pq.push(i);
    }
    EXPECT_EQ(pq.size(), 100u);
    EXPECT_EQ(pq.top(), 99);
}

// Тест: operator== (сравнение содержимого + режима)
TEST(PriorityQueueTest, EqualitySameContentSameMode) {
    PriorityQueue a(true), b(true);
    a.push(3); a.push(1); a.push(4);
    b.push(1); b.push(4); b.push(3);
    EXPECT_TRUE(a == b);
}

TEST(PriorityQueueTest, EqualityDifferentContent) {
    PriorityQueue a(true), b(true);
    a.push(1); a.push(2);
    b.push(1); b.push(3);
    EXPECT_FALSE(a == b);
}

TEST(PriorityQueueTest, EqualitySameContentDifferentMode) {
    PriorityQueue max_pq(true), min_pq(false);
    max_pq.push(1); max_pq.push(2);
    min_pq.push(1); min_pq.push(2);
    EXPECT_FALSE(max_pq == min_pq);
}

// Тест: operator!=
TEST(PriorityQueueTest, Inequality) {
    PriorityQueue a(true), b(true);
    a.push(1);
    b.push(2);
    EXPECT_TRUE(a != b);
}

// Тест: копирование — независимые объекты
TEST(PriorityQueueTest, CopyConstructor) {
    PriorityQueue original(true);
    original.push(10);
    original.push(30);

    PriorityQueue copy = original;
    EXPECT_TRUE(copy == original);
    EXPECT_EQ(copy.top(), 30);

    copy.pop();
    EXPECT_EQ(copy.top(), 10);
    EXPECT_EQ(original.top(), 30);
}

// Тест: перемещение — исходный объект остаётся в валидном состоянии
TEST(PriorityQueueTest, MoveConstructor) {
    PriorityQueue original(true);
    original.push(100);
    original.push(200);

    PriorityQueue moved = std::move(original);
    EXPECT_EQ(moved.top(), 200);
    EXPECT_TRUE(original.empty());
}

// Тест: присваивание копированием
TEST(PriorityQueueTest, CopyAssignment) {
    PriorityQueue a(true);
    a.push(7);

    PriorityQueue b(false);
    b.push(-1);
    b.push(-2);

    b = a;
    EXPECT_TRUE(b.is_max_heap());
    EXPECT_EQ(b.size(), 1u);
    EXPECT_EQ(b.top(), 7);
}

// Тест: присваивание перемещением
TEST(PriorityQueueTest, MoveAssignment) {
    PriorityQueue a(true);
    a.push(999);
    a.push(1);

    PriorityQueue b(false);
    b.push(-5);

    b = std::move(a);
    EXPECT_TRUE(b.is_max_heap());
    EXPECT_EQ(b.top(), 999);
    EXPECT_TRUE(a.empty());
}

// Тест: вывод в поток — формат "[a, b, c]"
TEST(PriorityQueueTest, StreamOutput) {
    PriorityQueue pq(true);
    pq.push(3);
    pq.push(1);
    pq.push(4);

    std::ostringstream oss;
    oss << pq;
    std::string s = oss.str();

    EXPECT_FALSE(s.empty());
    EXPECT_NE(s, "[]");
    EXPECT_TRUE(s.find("3") != std::string::npos);
    EXPECT_TRUE(s.find("1") != std::string::npos);
    EXPECT_TRUE(s.find("4") != std::string::npos);
}

// Тест: корректная работа с INT_MIN и INT_MAX
TEST(PriorityQueueTest, HandlesIntMinMax) {
    PriorityQueue max_pq(true);
    max_pq.push(INT_MAX);
    max_pq.push(INT_MIN);
    max_pq.push(0);
    EXPECT_EQ(max_pq.top(), INT_MAX);
    max_pq.pop();
    EXPECT_EQ(max_pq.top(), 0);
    max_pq.pop();
    EXPECT_EQ(max_pq.top(), INT_MIN);

    PriorityQueue min_pq(false);
    min_pq.push(INT_MAX);
    min_pq.push(INT_MIN);
    min_pq.push(0);
    EXPECT_EQ(min_pq.top(), INT_MIN);
    min_pq.pop();
    EXPECT_EQ(min_pq.top(), 0);
    min_pq.pop();
    EXPECT_EQ(min_pq.top(), INT_MAX);
}

// Тест: большая очередь (~10^4 элементов)
TEST(PriorityQueueTest, LargeQueue) {
    PriorityQueue pq(true);
    const int N = 10000;
    for (int i = 0; i < N; ++i) {
        pq.push(i);
    }
    EXPECT_EQ(pq.size(), static_cast<size_t>(N));
    EXPECT_EQ(pq.top(), N - 1);

    for (int i = N - 1; i >= 0; --i) {
        EXPECT_EQ(pq.top(), i);
        pq.pop();
    }
    EXPECT_TRUE(pq.empty());
}

// Тест: деструктор не падает (implicit — но проверим через scope)
TEST(PriorityQueueTest, Destructor) {
    {
        PriorityQueue pq;
        pq.push(1);
        pq.push(2);
    }
    SUCCEED();
}
