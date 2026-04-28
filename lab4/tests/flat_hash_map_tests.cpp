#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "FlatHashMap.hpp"

// Тесты конструкторов

// DefaultConstructor: контейнер пуст после создания по умолчанию
TEST(FlatHashMapTest, DefaultConstructor) {
  FlatHashMap<int, int> map;
  EXPECT_TRUE(map.empty());
  EXPECT_EQ(map.size(), 0);
}

// ConstructorWithBucketCount: конструктор с указанием минимального числа бакетов
TEST(FlatHashMapTest, ConstructorWithBucketCount) {
  FlatHashMap<int, int> map(32);
  EXPECT_TRUE(map.empty());
  EXPECT_GE(map.capacity(), 32);
}

// ConstructorFromRange: конструктор из диапазона итераторов
TEST(FlatHashMapTest, ConstructorFromRange) {
  std::vector<std::pair<const int, int>> data = {{1, 10}, {2, 20}, {3, 30}};
  FlatHashMap<int, int> map(data.begin(), data.end());
  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[1], 10);
  EXPECT_EQ(map[2], 20);
  EXPECT_EQ(map[3], 30);
}

// ConstructorFromInitializerList: инициализация через initializer_list
TEST(FlatHashMapTest, ConstructorFromInitializerList) {
  FlatHashMap<int, int> map = {{1, 10}, {2, 20}, {3, 30}};
  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(1), 10);
  EXPECT_EQ(map.at(2), 20);
  EXPECT_EQ(map.at(3), 30);
}

// CopyConstructor: копирование контейнера
TEST(FlatHashMapTest, CopyConstructor) {
  FlatHashMap<int, int> original = {{1, 10}, {2, 20}};
  FlatHashMap<int, int> copy(original);
  EXPECT_EQ(copy.size(), original.size());
  EXPECT_EQ(copy[1], 10);
  EXPECT_EQ(copy[2], 20);
  original[1] = 100;
  EXPECT_EQ(copy[1], 10);
}

// MoveConstructor: перемещение контейнера
TEST(FlatHashMapTest, MoveConstructor) {
  FlatHashMap<int, int> original = {{1, 10}, {2, 20}};
  FlatHashMap<int, int> moved(std::move(original));
  EXPECT_EQ(moved.size(), 2);
  EXPECT_EQ(moved[1], 10);
  EXPECT_TRUE(original.empty());
}

// Тесты операторов присвоения

// CopyAssignment: оператор копирования
TEST(FlatHashMapTest, CopyAssignment) {
  FlatHashMap<int, int> original = {{1, 10}, {2, 20}};
  FlatHashMap<int, int> copy;
  copy = original;
  EXPECT_EQ(copy.size(), 2);
  EXPECT_EQ(copy[1], 10);
}

// MoveAssignment: оператор перемещения
TEST(FlatHashMapTest, MoveAssignment) {
  FlatHashMap<int, int> original = {{1, 10}, {2, 20}};
  FlatHashMap<int, int> moved;
  moved = std::move(original);
  EXPECT_EQ(moved.size(), 2);
  EXPECT_TRUE(original.empty());
}

// InitializerListAssignment: присвоение через initializer_list
TEST(FlatHashMapTest, InitializerListAssignment) {
  FlatHashMap<int, int> map;
  map = {{1, 10}, {2, 20}, {3, 30}};
  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[2], 20);
}

// Тесты swap

// Swap: обмен содержимого между контейнерами
TEST(FlatHashMapTest, Swap) {
  FlatHashMap<int, int> map1 = {{1, 10}};
  FlatHashMap<int, int> map2 = {{2, 20}, {3, 30}};
  map1.swap(map2);
  EXPECT_EQ(map1.size(), 2);
  EXPECT_EQ(map2.size(), 1);
  EXPECT_EQ(map1[2], 20);
  EXPECT_EQ(map2[1], 10);
}

// Тесты итераторов

// IteratorBeginEnd: проверка прохода итератора от begin до end
TEST(FlatHashMapTest, IteratorBeginEnd) {
  FlatHashMap<int, int> map = {{1, 10}, {2, 20}, {3, 30}};
  int count = 0;
  int sum_keys = 0;
  int sum_values = 0;
  for (auto it = map.begin(); it != map.end(); ++it) {
    ++count;
    sum_keys += it->first;
    sum_values += it->second;
  }
  EXPECT_EQ(count, 3);
  EXPECT_EQ(sum_keys, 1 + 2 + 3);
  EXPECT_EQ(sum_values, 10 + 20 + 30);
}

// ConstIterator: проверка const-итератора cbegin/cend
TEST(FlatHashMapTest, ConstIterator) {
  const FlatHashMap<int, int> map = {{1, 10}, {2, 20}};
  int count = 0;
  for (auto it = map.cbegin(); it != map.cend(); ++it) {
    ++count;
  }
  EXPECT_EQ(count, 2);
}

// RangeBasedFor: поддержка range-based for (диапазонный for)
TEST(FlatHashMapTest, RangeBasedFor) {
  FlatHashMap<std::string, int> map = {{"a", 1}, {"b", 2}};
  int sum = 0;
  for (const auto& [k, v] : map) {
    sum += v;
  }
  EXPECT_EQ(sum, 3);
}

// EmptyMapIterators: итераторы на пустом контейнере равны
TEST(FlatHashMapTest, EmptyMapIterators) {
  FlatHashMap<int, int> map;
  EXPECT_EQ(map.begin(), map.end());
}

// Тесты empty, size, clear

// EmptyAndSize: проверка пустоты и размера при вставке
TEST(FlatHashMapTest, EmptyAndSize) {
  FlatHashMap<int, int> map;
  EXPECT_TRUE(map.empty());
  EXPECT_EQ(map.size(), 0);
  map[1] = 10;
  EXPECT_FALSE(map.empty());
  EXPECT_EQ(map.size(), 1);
}

// Clear: очистка контейнера
TEST(FlatHashMapTest, Clear) {
  FlatHashMap<int, int> map = {{1, 10}, {2, 20}};
  map.clear();
  EXPECT_TRUE(map.empty());
  EXPECT_EQ(map.size(), 0);
  EXPECT_EQ(map.find(1), map.end());
}

// Тесты insert
// InsertSingleValue: вставка одного значения
TEST(FlatHashMapTest, InsertSingleValue) {
  FlatHashMap<int, int> map;
  auto [it, inserted] = map.insert({1, 10});
  EXPECT_TRUE(inserted);
  EXPECT_EQ(it->first, 1);
  EXPECT_EQ(it->second, 10);
}

// InsertDuplicateKey: попытка вставить дубликат ключа
TEST(FlatHashMapTest, InsertDuplicateKey) {
  FlatHashMap<int, int> map;
  map.insert({1, 10});
  auto [it, inserted] = map.insert({1, 20});
  EXPECT_FALSE(inserted);
  EXPECT_EQ(it->second, 10);
}

// InsertMoveValue: вставка с перемещением значения
TEST(FlatHashMapTest, InsertMoveValue) {
  FlatHashMap<int, std::string> map;
  std::string s = "hello";
  auto [it, inserted] = map.insert({1, std::move(s)});
  EXPECT_TRUE(inserted);
  EXPECT_EQ(it->second, "hello");
}

// InsertRange: вставка через диапазон итераторов
TEST(FlatHashMapTest, InsertRange) {
  FlatHashMap<int, int> map;
  std::vector<std::pair<const int, int>> data = {{1, 10}, {2, 20}};
  map.insert(data.begin(), data.end());
  EXPECT_EQ(map.size(), 2);
}

// InsertInitializerList: вставка через initializer_list
TEST(FlatHashMapTest, InsertInitializerList) {
  FlatHashMap<int, int> map;
  map.insert({{1, 10}, {2, 20}, {3, 30}});
  EXPECT_EQ(map.size(), 3);
}

// Тесты emplace
// Emplace: размещение элемента с конструированием на месте
TEST(FlatHashMapTest, Emplace) {
  FlatHashMap<int, std::string> map;
  auto [it, inserted] = map.emplace(1, "hello");
  EXPECT_TRUE(inserted);
  EXPECT_EQ(it->second, "hello");
}

// EmplaceDuplicate: emplace с уже существующим ключом
TEST(FlatHashMapTest, EmplaceDuplicate) {
  FlatHashMap<int, int> map;
  map.emplace(1, 10);
  auto [it, inserted] = map.emplace(1, 20);
  EXPECT_FALSE(inserted);
  EXPECT_EQ(it->second, 10);
}

// Тесты erase
// EraseByKey: удаление по ключу
TEST(FlatHashMapTest, EraseByKey) {
  FlatHashMap<int, int> map = {{1, 10}, {2, 20}, {3, 30}};
  EXPECT_EQ(map.erase(2), 1);
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.find(2), map.end());
}

// EraseNonExistentKey: удаление несуществующего ключа
TEST(FlatHashMapTest, EraseNonExistentKey) {
  FlatHashMap<int, int> map = {{1, 10}};
  EXPECT_EQ(map.erase(999), 0);
  EXPECT_EQ(map.size(), 1);
}

// EraseByIterator: удаление через итератор
TEST(FlatHashMapTest, EraseByIterator) {
  FlatHashMap<int, int> map = {{1, 10}, {2, 20}};
  auto it = map.find(1);
  ASSERT_NE(it, map.end());
  auto next = map.erase(it);
  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map.find(1), map.end());
}

// EraseAndReinsert: удаление и повторная вставка в тот же ключ
TEST(FlatHashMapTest, EraseAndReinsert) {
  FlatHashMap<int, int> map;
  map[1] = 10;
  map.erase(1);
  map[1] = 20;
  EXPECT_EQ(map[1], 20);
}

// Тесты reserve и capacity

// Reserve: резервирование ёмкости
TEST(FlatHashMapTest, Reserve) {
  FlatHashMap<int, int> map;
  map.reserve(100);
  EXPECT_GE(map.capacity(), 100);
  EXPECT_TRUE(map.empty());
}

// CapacityAfterInserts: проверка capacity после множества вставок
TEST(FlatHashMapTest, CapacityAfterInserts) {
  FlatHashMap<int, int> map;
  for (int i = 0; i < 50; ++i) {
    map[i] = i * 10;
  }
  EXPECT_EQ(map.size(), 50);
  EXPECT_GE(map.capacity(), 50);
}

// Тесты load_factor и max_load_factor

// LoadFactor: проверка вычисления коэффициента загрузки
TEST(FlatHashMapTest, LoadFactor) {
  FlatHashMap<int, int> map;
  EXPECT_FLOAT_EQ(map.load_factor(), 0.0f);
  map.reserve(16);
  map[1] = 10;
  EXPECT_GT(map.load_factor(), 0.0f);
}

// MaxLoadFactor: чтение/запись предела загрузки
TEST(FlatHashMapTest, MaxLoadFactor) {
  FlatHashMap<int, int> map;
  float default_mlf = map.max_load_factor();
  EXPECT_GT(default_mlf, 0.0f);
  EXPECT_LE(default_mlf, 1.0f);

  map.max_load_factor(0.5f);
  EXPECT_FLOAT_EQ(map.max_load_factor(), 0.5f);
}

// Тесты operator

// OperatorBracketInsert: вставка через оператор[]
TEST(FlatHashMapTest, OperatorBracketInsert) {
  FlatHashMap<int, int> map;
  map[1] = 10;
  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map[1], 10);
}

// OperatorBracketDefaultValue: оператор[] возвращает дефолтное значение
TEST(FlatHashMapTest, OperatorBracketDefaultValue) {
  FlatHashMap<int, int> map;
  int val = map[1];
  EXPECT_EQ(val, 0);
  EXPECT_EQ(map.size(), 1);
}

// OperatorBracketUpdate: обновление значения через оператор[]
TEST(FlatHashMapTest, OperatorBracketUpdate) {
  FlatHashMap<int, int> map;
  map[1] = 10;
  map[1] = 20;
  EXPECT_EQ(map[1], 20);
  EXPECT_EQ(map.size(), 1);
}

// OperatorBracketWithMoveKey: использование move-ключа в operator[]
TEST(FlatHashMapTest, OperatorBracketWithMoveKey) {
  FlatHashMap<std::string, int> map;
  std::string key = "hello";
  map[std::move(key)] = 42;
  EXPECT_EQ(map["hello"], 42);
}

// Тесты at

// AtExistingKey: доступ через at для существующего ключа
TEST(FlatHashMapTest, AtExistingKey) {
  FlatHashMap<int, int> map = {{1, 10}, {2, 20}};
  EXPECT_EQ(map.at(1), 10);
  EXPECT_EQ(map.at(2), 20);
}

// AtNonExistentKey: at бросает при отсутствии ключа
TEST(FlatHashMapTest, AtNonExistentKey) {
  FlatHashMap<int, int> map = {{1, 10}};
  EXPECT_THROW(map.at(999), std::out_of_range);
}

// AtConst: const-вызов at
TEST(FlatHashMapTest, AtConst) {
  const FlatHashMap<int, int> map = {{1, 10}};
  EXPECT_EQ(map.at(1), 10);
  EXPECT_THROW(map.at(999), std::out_of_range);
}

// AtModifyValue: изменение значения через at()
TEST(FlatHashMapTest, AtModifyValue) {
  FlatHashMap<int, int> map = {{1, 10}};
  map.at(1) = 20;
  EXPECT_EQ(map.at(1), 20);
}

// Тесты find

// FindExisting: поиск существующего ключа
TEST(FlatHashMapTest, FindExisting) {
  FlatHashMap<int, int> map = {{1, 10}, {2, 20}};
  auto it = map.find(1);
  ASSERT_NE(it, map.end());
  EXPECT_EQ(it->first, 1);
  EXPECT_EQ(it->second, 10);
}

// FindNonExistent: поиск несуществующего ключа возвращает end
TEST(FlatHashMapTest, FindNonExistent) {
  FlatHashMap<int, int> map = {{1, 10}};
  auto it = map.find(999);
  EXPECT_EQ(it, map.end());
}

// FindConst: const-версия find
TEST(FlatHashMapTest, FindConst) {
  const FlatHashMap<int, int> map = {{1, 10}};
  auto it = map.find(1);
  ASSERT_NE(it, map.end());
  EXPECT_EQ(it->second, 10);
}

// Тесты contains

// ContainsTrue: contains возвращает true для существующего ключа
TEST(FlatHashMapTest, ContainsTrue) {
  FlatHashMap<int, int> map = {{1, 10}};
  EXPECT_TRUE(map.contains(1));
}

// ContainsFalse: contains возвращает false для отсутствующего ключа
TEST(FlatHashMapTest, ContainsFalse) {
  FlatHashMap<int, int> map = {{1, 10}};
  EXPECT_FALSE(map.contains(999));
}

// Тесты count

// CountExisting: count возвращает 1 для существующих ключей
TEST(FlatHashMapTest, CountExisting) {
  FlatHashMap<int, int> map = {{1, 10}, {2, 20}};
  EXPECT_EQ(map.count(1), 1);
  EXPECT_EQ(map.count(2), 1);
}

// CountNonExistent: count возвращает 0 для отсутствующего ключа
TEST(FlatHashMapTest, CountNonExistent) {
  FlatHashMap<int, int> map = {{1, 10}};
  EXPECT_EQ(map.count(999), 0);
}

// Тесты со строками

// StringKeys: использование строковых ключей
TEST(FlatHashMapTest, StringKeys) {
  FlatHashMap<std::string, int> map;
  map["hello"] = 1;
  map["world"] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map["hello"], 1);
  EXPECT_EQ(map["world"], 2);
}

// StringValues: использование строковых значений
TEST(FlatHashMapTest, StringValues) {
  FlatHashMap<int, std::string> map;
  map[1] = "one";
  map[2] = "two";
  EXPECT_EQ(map[1], "one");
  EXPECT_EQ(map[2], "two");
}

// Тесты стресс-тесты

// ManyInserts: много вставок и проверка значений
TEST(FlatHashMapTest, ManyInserts) {
  FlatHashMap<int, int> map;
  constexpr int N = 10000;
  for (int i = 0; i < N; ++i) {
    map[i] = i * 2;
  }
  EXPECT_EQ(map.size(), N);
  for (int i = 0; i < N; ++i) {
    ASSERT_EQ(map[i], i * 2);
  }
}

// ManyInsertsAndDeletes: вставки и удаления, проверка contains
TEST(FlatHashMapTest, ManyInsertsAndDeletes) {
  FlatHashMap<int, int> map;
  constexpr int N = 1000;
  for (int i = 0; i < N; ++i) {
    map[i] = i;
  }
  for (int i = 0; i < N / 2; ++i) {
    map.erase(i);
  }
  EXPECT_EQ(map.size(), N / 2);
  for (int i = N / 2; i < N; ++i) {
    EXPECT_TRUE(map.contains(i));
  }
}

// CollisionHandling: проверка обработки коллизий с плохой хеш-функцией
TEST(FlatHashMapTest, CollisionHandling) {
  struct BadHash {
    size_t operator()(int key) const { return key % 4; }
  };
  FlatHashMap<int, int, BadHash> map;
  for (int i = 0; i < 100; ++i) {
    map[i] = i * 10;
  }
  EXPECT_EQ(map.size(), 100);
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(map[i], i * 10);
  }
}

// Тесты граничных условий

// EraseFromEmptyMap: erase на пустом контейнере возвращает 0
TEST(FlatHashMapTest, EraseFromEmptyMap) {
  FlatHashMap<int, int> map;
  EXPECT_EQ(map.erase(1), 0);
}

// FindInEmptyMap: find в пустом контейнере возвращает end
TEST(FlatHashMapTest, FindInEmptyMap) {
  FlatHashMap<int, int> map;
  EXPECT_EQ(map.find(1), map.end());
}

// ClearEmptyMap: clear на пустом контейнере безопасен
TEST(FlatHashMapTest, ClearEmptyMap) {
  FlatHashMap<int, int> map;
  map.clear();
  EXPECT_TRUE(map.empty());
}

// SelfAssignment: самоприсваивание
TEST(FlatHashMapTest, SelfAssignment) {
  FlatHashMap<int, int> map = {{1, 10}};
  map = map;
  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map[1], 10);
}

// Тест SIMD (косвенный — проверка корректности при разных размерах)

// SimdGroupBoundaries: проверка на границе первой группы (16 элементов)
TEST(FlatHashMapTest, SimdGroupBoundaries) {
  FlatHashMap<int, int> map;
  for (int i = 0; i < 16; ++i) {
    map[i] = i * 100;
  }
  EXPECT_EQ(map.size(), 16);
  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(map[i], i * 100);
  }
}

// SimdMultipleGroups: проверка нескольких SIMD-групп
TEST(FlatHashMapTest, SimdMultipleGroups) {
  FlatHashMap<int, int> map;
  for (int i = 0; i < 64; ++i) {
    map[i] = i;
  }
  EXPECT_EQ(map.size(), 64);
  for (int i = 0; i < 64; ++i) {
    auto it = map.find(i);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->second, i);
  }
}
