#pragma once

#include <emmintrin.h>
#include <cstdint>
#include <cstring>
#include <functional>  // std::hash, std::equal_to
#include <initializer_list>
#include <stdexcept>
#include <utility>  // std::pair

// Элементы хранятся в массиве (плоское хранение), коллизии
// разрешаются методом открытой адресации.
template <
  class Key,
  class T,
  class Hash = std::hash<Key>,
  class KeyEqual = std::equal_to<Key>
>
class FlatHashMap {
 public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using hasher = Hash;
  using key_equal = KeyEqual;

  // Итераторы должны быть как минимум однонаправленными (ForwardIterator).
  class iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = FlatHashMap::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    iterator() = default;
    iterator(const iterator&) = default;
    iterator& operator=(const iterator&) = default;

    reference operator*() const { return *slot_; }
    pointer operator->() const { return slot_; }

    iterator& operator++() {
      ++ctrl_;
      ++slot_;
      SkipEmptyOrDeleted();
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const iterator& other) const { return ctrl_ == other.ctrl_; }
    bool operator!=(const iterator& other) const { return ctrl_ != other.ctrl_; }

   private:
    friend class FlatHashMap;
    friend class const_iterator;

    iterator(int8_t* ctrl, value_type* slot, int8_t* ctrl_end)
        : ctrl_(ctrl), slot_(slot), ctrl_end_(ctrl_end) {}

    void SkipEmptyOrDeleted() {
      while (ctrl_ < ctrl_end_ && *ctrl_ < 0) {
        ++ctrl_;
        ++slot_;
      }
    }

    int8_t* ctrl_ = nullptr;
    value_type* slot_ = nullptr;
    int8_t* ctrl_end_ = nullptr;
  };

  class const_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const FlatHashMap::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const FlatHashMap::value_type*;
    using reference = const FlatHashMap::value_type&;

    const_iterator() = default;
    const_iterator(const const_iterator&) = default;
    const_iterator& operator=(const const_iterator&) = default;
    const_iterator(const iterator& it)
        : ctrl_(it.ctrl_), slot_(it.slot_), ctrl_end_(it.ctrl_end_) {}

    reference operator*() const { return *slot_; }
    pointer operator->() const { return slot_; }

    const_iterator& operator++() {
      ++ctrl_;
      ++slot_;
      SkipEmptyOrDeleted();
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const const_iterator& other) const {
      return ctrl_ == other.ctrl_;
    }
    bool operator!=(const const_iterator& other) const {
      return ctrl_ != other.ctrl_;
    }

   private:
    friend class FlatHashMap;

    const_iterator(const int8_t* ctrl, const FlatHashMap::value_type* slot,
                   const int8_t* ctrl_end)
        : ctrl_(ctrl), slot_(slot), ctrl_end_(ctrl_end) {}

    void SkipEmptyOrDeleted() {
      while (ctrl_ < ctrl_end_ && *ctrl_ < 0) {
        ++ctrl_;
        ++slot_;
      }
    }

    const int8_t* ctrl_ = nullptr;
    const FlatHashMap::value_type* slot_ = nullptr;
    const int8_t* ctrl_end_ = nullptr;
  };

  // Конструирует пустой контейнер.
  FlatHashMap() = default;

  // Конструирует контейнер с заданным количеством "бакетов"
  // Реальная ёмкость может отличаться, но не должна быть меньше bucket_count.
  explicit FlatHashMap(size_type bucket_count,
                         const Hash& hash = Hash(),
                         const KeyEqual& equal = KeyEqual())
      : hash_fn_(hash), equal_fn_(equal) {
    if (bucket_count > 0) {
      AllocateTable(NextPowerOfTwo(
          bucket_count < kGroupSize ? kGroupSize : bucket_count));
    }
  }

  // Конструирует контейнер, заполняя его элементами из диапазона [first, last).
  template <class InputIt>
  FlatHashMap(InputIt first, InputIt last,
                size_type bucket_count = 0,
                const Hash& hash = Hash(),
                const KeyEqual& equal = KeyEqual())
      : hash_fn_(hash), equal_fn_(equal) {
    if (bucket_count > 0) {
      AllocateTable(NextPowerOfTwo(
          bucket_count < kGroupSize ? kGroupSize : bucket_count));
    }
    insert(first, last);
  }

  // Конструирует контейнер из списка инициализации.
  FlatHashMap(std::initializer_list<value_type> init,
                size_type bucket_count = 0,
                const Hash& hash = Hash(),
                const KeyEqual& equal = KeyEqual())
      : hash_fn_(hash), equal_fn_(equal) {
    size_type min_cap = bucket_count > 0 ? bucket_count : init.size();
    if (min_cap > 0) {
      AllocateTable(NextPowerOfTwo(min_cap < kGroupSize ? kGroupSize : min_cap));
    }
    insert(init);
  }

  FlatHashMap(const FlatHashMap& other)
      : size_(0),
        capacity_(0),
        max_load_factor_(other.max_load_factor_),
        hash_fn_(other.hash_fn_),
        equal_fn_(other.equal_fn_) {
    if (other.capacity_ > 0) {
      AllocateTable(other.capacity_);
      for (size_type i = 0; i < other.capacity_; ++i) {
        if (IsOccupied(other.ctrl_[i])) {
          ctrl_[i] = other.ctrl_[i];
          new (slots_ + i) value_type(other.slots_[i]);
          ++size_;
        }
      }
    }
  }

  FlatHashMap(FlatHashMap&& other) noexcept
      : ctrl_(other.ctrl_),
        slots_(other.slots_),
        size_(other.size_),
        capacity_(other.capacity_),
        max_load_factor_(other.max_load_factor_),
        hash_fn_(std::move(other.hash_fn_)),
        equal_fn_(std::move(other.equal_fn_)) {
    other.ctrl_ = nullptr;
    other.slots_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  ~FlatHashMap() { DeallocateTable(); }

  FlatHashMap& operator=(const FlatHashMap& other) {
    if (this != &other) {
      FlatHashMap tmp(other);
      swap(tmp);
    }
    return *this;
  }

  FlatHashMap& operator=(FlatHashMap&& other) noexcept {
    if (this != &other) {
      DeallocateTable();
      ctrl_ = other.ctrl_;
      slots_ = other.slots_;
      size_ = other.size_;
      capacity_ = other.capacity_;
      max_load_factor_ = other.max_load_factor_;
      hash_fn_ = std::move(other.hash_fn_);
      equal_fn_ = std::move(other.equal_fn_);
      other.ctrl_ = nullptr;
      other.slots_ = nullptr;
      other.size_ = 0;
      other.capacity_ = 0;
    }
    return *this;
  }

  FlatHashMap& operator=(std::initializer_list<value_type> init) {
    clear();
    insert(init);
    return *this;
  }

  // Обменивает содержимое двух контейнеров.
  void swap(FlatHashMap& other) noexcept {
    std::swap(ctrl_, other.ctrl_);
    std::swap(slots_, other.slots_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(max_load_factor_, other.max_load_factor_);
    std::swap(hash_fn_, other.hash_fn_);
    std::swap(equal_fn_, other.equal_fn_);
  }

  // Итераторы

  iterator begin() noexcept {
    if (capacity_ == 0) return end();
    iterator it(ctrl_, slots_, ctrl_ + capacity_);
    if (*it.ctrl_ < 0) it.SkipEmptyOrDeleted();
    return it;
  }

  const_iterator begin() const noexcept {
    if (capacity_ == 0) return end();
    const_iterator it(ctrl_, slots_, ctrl_ + capacity_);
    if (*it.ctrl_ < 0) it.SkipEmptyOrDeleted();
    return it;
  }

  const_iterator cbegin() const noexcept { return begin(); }

  iterator end() noexcept {
    return iterator(ctrl_ + capacity_, slots_ + capacity_, ctrl_ + capacity_);
  }

  const_iterator end() const noexcept {
    return const_iterator(ctrl_ + capacity_, slots_ + capacity_, ctrl_ + capacity_);
  }

  const_iterator cend() const noexcept { return end(); }

  //Размер и очистка

  bool empty() const noexcept { return size_ == 0; }

  size_type size() const noexcept { return size_; }

  void clear() noexcept {
    if (ctrl_) {
      for (size_type i = 0; i < capacity_; ++i) {
        if (IsOccupied(ctrl_[i])) {
          slots_[i].~value_type();
          SetCtrl(i, kEmpty);
        } else if (ctrl_[i] == kDeleted) {
          SetCtrl(i, kEmpty);
        }
      }
      size_ = 0;
    }
  }

  //Вставка

  std::pair<iterator, bool> insert(const value_type& value) {
    return InsertImpl(value);
  }

  std::pair<iterator, bool> insert(value_type&& value) {
    return InsertImpl(std::move(value));
  }

  template <class InputIt>
  void insert(InputIt first, InputIt last) {
    for (; first != last; ++first) {
      insert(*first);
    }
  }

  void insert(std::initializer_list<value_type> init) {
    for (const auto& v : init) {
      insert(v);
    }
  }

  template <class... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    //создаём временную пару
    value_type tmp(std::forward<Args>(args)...);
    return InsertImpl(std::move(tmp));
  }

  //Удаление

  size_type erase(const key_type& key) {
    size_type idx = FindIndex(key);
    if (idx == capacity_) return 0;
    slots_[idx].~value_type();
    SetCtrl(idx, kDeleted);
    --size_;
    return 1;
  }

  iterator erase(iterator pos) {
    if (pos == end()) return end();
    size_type idx = pos.ctrl_ - ctrl_;
    slots_[idx].~value_type();
    SetCtrl(idx, kDeleted);
    --size_;
    ++pos;
    return pos;
  }

  //Ёмкость

  void reserve(size_type new_capacity) {
    size_type required = CapacityForSize(new_capacity);
    if (required > capacity_) {
      Rehash(required);
    }
  }

  size_type capacity() const noexcept { return capacity_; }

  float load_factor() const noexcept {
    return capacity_ == 0 ? 0.0f : static_cast<float>(size_) / capacity_;
  }

  void max_load_factor(float ml) {
    if (ml > 0 && ml <= 1.0f) {
      max_load_factor_ = ml;
    }
  }

  float max_load_factor() const noexcept { return max_load_factor_; }

  //Доступ по ключу

  mapped_type& operator[](const key_type& key) {
    MaybeGrow();
    auto [idx, found] = FindInsertSlot(key);
    if (!found) {
      size_t hash = hash_fn_(key);
      SetCtrl(idx, H2(hash));
      new (slots_ + idx) value_type(key, mapped_type{});
      ++size_;
    }
    return slots_[idx].second;
  }

  mapped_type& operator[](key_type&& key) {
    MaybeGrow();
    auto [idx, found] = FindInsertSlot(key);
    if (!found) {
      size_t hash = hash_fn_(key);
      SetCtrl(idx, H2(hash));
      new (slots_ + idx) value_type(std::move(key), mapped_type{});
      ++size_;
    }
    return slots_[idx].second;
  }

  mapped_type& at(const key_type& key) {
    size_type idx = FindIndex(key);
    if (idx == capacity_) {
      throw std::out_of_range("FlatHashMap::at: key not found");
    }
    return slots_[idx].second;
  }

  const mapped_type& at(const key_type& key) const {
    size_type idx = FindIndex(key);
    if (idx == capacity_) {
      throw std::out_of_range("FlatHashMap::at: key not found");
    }
    return slots_[idx].second;
  }

  //Поиск

  iterator find(const key_type& key) {
    size_type idx = FindIndex(key);
    if (idx == capacity_) return end();
    return iterator(ctrl_ + idx, slots_ + idx, ctrl_ + capacity_);
  }

  const_iterator find(const key_type& key) const {
    size_type idx = FindIndex(key);
    if (idx == capacity_) return end();
    return const_iterator(ctrl_ + idx, slots_ + idx, ctrl_ + capacity_);
  }

  bool contains(const key_type& key) const {
    return FindIndex(key) != capacity_;
  }

  size_type count(const key_type& key) const {
    return contains(key) ? 1 : 0;
  }

 private:
  //control-байты
  static constexpr int8_t kEmpty = -128;    //пустая ячейка
  static constexpr int8_t kDeleted = -2;    //удалённая
  static constexpr size_type kGroupSize = 16;
  static constexpr size_type kDefaultCapacity = 16;
  static constexpr float kDefaultMaxLoadFactor = 0.875f;

  int8_t* ctrl_ = nullptr;       //массив control-байтов
  value_type* slots_ = nullptr;  //массив для ключ-значение
  size_type size_ = 0;           //кол-во элементов
  size_type capacity_ = 0;       //кол-во бакетов
  float max_load_factor_ = kDefaultMaxLoadFactor;
  Hash hash_fn_;
  KeyEqual equal_fn_;

  static int8_t H2(size_t hash) {
    return static_cast<int8_t>(hash >> (sizeof(size_t) * 8 - 7));
  }

  //начальный индекс бакета
  size_type H1(size_t hash) const {
    return hash & (capacity_ - 1);
  }

  static bool IsOccupied(int8_t ctrl) {
    return ctrl >= 0;
  }

  static bool IsEmptyOrDeleted(int8_t ctrl) {
    return ctrl < 0;
  }

  //поиск совпадений h2 в группе из 16 control-байтов
  static uint32_t MatchH2(const int8_t* group, int8_t h2) {
    __m128i ctrl_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(group));
    __m128i h2_vec = _mm_set1_epi8(h2);
    __m128i cmp = _mm_cmpeq_epi8(ctrl_vec, h2_vec);
    return static_cast<uint32_t>(_mm_movemask_epi8(cmp));
  }

  //поиск пустых ячеек
  static uint32_t MatchEmpty(const int8_t* group) {
    __m128i ctrl_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(group));
    __m128i empty_vec = _mm_set1_epi8(kEmpty);
    __m128i cmp = _mm_cmpeq_epi8(ctrl_vec, empty_vec);
    return static_cast<uint32_t>(_mm_movemask_epi8(cmp));
  }

  //поиск пустых или удалённых ячеек
  static uint32_t MatchEmptyOrDeleted(const int8_t* group) {
    __m128i ctrl_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(group));
    //< 0 это EMPTY или DELETED
    //kEmpty = -128
    __m128i zero = _mm_setzero_si128();
    __m128i cmp = _mm_cmplt_epi8(ctrl_vec, zero);
    return static_cast<uint32_t>(_mm_movemask_epi8(cmp));
  }

  static int CountTrailingZeros(uint32_t mask) {
    unsigned long index;
    _BitScanForward(&index, mask);
    return static_cast<int>(index);
  }
  void SetCtrl(size_type idx, int8_t value) {
    ctrl_[idx] = value;
    if (idx < kGroupSize) {
      ctrl_[capacity_ + idx] = value;
    }
  }

  void AllocateTable(size_type capacity) {
    capacity_ = capacity;
    //выделяем capacity
    ctrl_ = new int8_t[capacity_ + kGroupSize];
    std::memset(ctrl_, kEmpty, capacity_ + kGroupSize);
    slots_ = reinterpret_cast<value_type*>(
        ::operator new(capacity_ * sizeof(value_type)));
  }

  void DeallocateTable() {
    if (ctrl_) {
      for (size_type i = 0; i < capacity_; ++i) {
        if (IsOccupied(ctrl_[i])) {
          slots_[i].~value_type();
        }
      }
      delete[] ctrl_;
      ::operator delete(slots_);
      ctrl_ = nullptr;
      slots_ = nullptr;
      size_ = 0;
      capacity_ = 0;
    }
  }

  static size_type NextPowerOfTwo(size_type n) {
    if (n == 0) return 1;
    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if constexpr (sizeof(size_type) > 4) {
      n |= n >> 32;
    }
    return n + 1;
  }

  //мин ёмкость для заданного числа элем
  size_type CapacityForSize(size_type n) const {
    if (n == 0) return 0;
    size_type min_cap = static_cast<size_type>(n / max_load_factor_) + 1;
    return NextPowerOfTwo(min_cap < kGroupSize ? kGroupSize : min_cap);
  }

  //Поиск

  //возвращает индекс найденного элемента
  size_type FindIndex(const key_type& key) const {
    if (capacity_ == 0) return capacity_;

    size_t hash = hash_fn_(key);
    int8_t h2 = H2(hash);
    size_type idx = H1(hash);

    while (true) {
      const int8_t* group = ctrl_ + idx;
      uint32_t match = MatchH2(group, h2);

      while (match) {
        int offset = CountTrailingZeros(match);
        size_type pos = (idx + offset) & (capacity_ - 1);
        if (equal_fn_(slots_[pos].first, key)) {
          return pos;
        }
        match &= (match - 1);  //сбросить младший бит
      }

      uint32_t empty_mask = MatchEmpty(group);
      if (empty_mask) {
        return capacity_;  //не найден
      }

      idx = (idx + kGroupSize) & (capacity_ - 1);
    }
  }

  //Поиск слота для вставки
  std::pair<size_type, bool> FindInsertSlot(const key_type& key) {
    size_t hash = hash_fn_(key);
    int8_t h2 = H2(hash);
    size_type idx = H1(hash);
    size_type first_deleted = capacity_;  //индекс первого DELETED

    while (true) {
      const int8_t* group = ctrl_ + idx;

      //Проверяем совпадения H2 — возможно, ключ уже существует
      uint32_t match = MatchH2(group, h2);
      while (match) {
        int offset = CountTrailingZeros(match);
        size_type pos = (idx + offset) & (capacity_ - 1);
        if (equal_fn_(slots_[pos].first, key)) {
          return {pos, true};  //элемент уже существует
        }
        match &= (match - 1);
      }

      //Ищем EMPTY или DELETED в этой группе
      uint32_t empty_or_del = MatchEmptyOrDeleted(group);
      while (empty_or_del) {
        int offset = CountTrailingZeros(empty_or_del);
        size_type pos = (idx + offset) & (capacity_ - 1);
        int8_t c = ctrl_[pos];
        if (c == kEmpty) {
          //Нашли пустую — конец probe-цепочки
          return {first_deleted == capacity_ ? pos : first_deleted, false};
        }
        if (c == kDeleted && first_deleted == capacity_) {
          first_deleted = pos;  //запоминаем первый DELETED
        }
        empty_or_del &= (empty_or_del - 1);
      }

      //В группе не было EMPTY
      idx = (idx + kGroupSize) & (capacity_ - 1);
    }
  }

  //Rehash в новую таблицу
  void Rehash(size_type new_capacity) {
    if (new_capacity < kGroupSize) {
      new_capacity = kGroupSize;
    }
    new_capacity = NextPowerOfTwo(new_capacity);

    int8_t* old_ctrl = ctrl_;
    value_type* old_slots = slots_;
    size_type old_capacity = capacity_;

    AllocateTable(new_capacity);
    size_ = 0;

    if (old_ctrl) {
      for (size_type i = 0; i < old_capacity; ++i) {
        if (IsOccupied(old_ctrl[i])) {
          size_t hash = hash_fn_(old_slots[i].first);
          int8_t h2 = H2(hash);
          size_type idx = H1(hash);

          while (true) {
            uint32_t empty_mask = MatchEmpty(ctrl_ + idx);
            if (empty_mask) {
              int offset = CountTrailingZeros(empty_mask);
              size_type pos = (idx + offset) & (capacity_ - 1);
              SetCtrl(pos, h2);
              new (slots_ + pos) value_type(std::move(old_slots[i]));
              old_slots[i].~value_type();
              ++size_;
              break;
            }
            idx = (idx + kGroupSize) & (capacity_ - 1);
          }
        }
      }
      delete[] old_ctrl;
      ::operator delete(old_slots);
    }
  }

  //Общая реализация вставки
  template <class V>
  std::pair<iterator, bool> InsertImpl(V&& value) {
    MaybeGrow();
    auto [idx, found] = FindInsertSlot(value.first);
    if (found) {
      return {iterator(ctrl_ + idx, slots_ + idx, ctrl_ + capacity_), false};
    }
    size_t hash = hash_fn_(value.first);
    SetCtrl(idx, H2(hash));
    new (slots_ + idx) value_type(std::forward<V>(value));
    ++size_;
    return {iterator(ctrl_ + idx, slots_ + idx, ctrl_ + capacity_), true};
  }

  //Проверка необходимости rehash перед вставкой
  void MaybeGrow() {
    if (capacity_ == 0) {
      AllocateTable(kDefaultCapacity);
      return;
    }
    if (static_cast<float>(size_ + 1) / capacity_ > max_load_factor_) {
      Rehash(capacity_ * 2);
    }
  }
};
