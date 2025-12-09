#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_

#include <vector>
#include <ostream>
#include <stdexcept>

class PriorityQueue {
public:
  using value_type = int;
  using size_type = std::size_t;

  PriorityQueue();
  explicit PriorityQueue(bool max_heap);
  explicit PriorityQueue(const std::vector<value_type>& data, bool max_heap = true);
  ~PriorityQueue() = default;

  PriorityQueue(const PriorityQueue& other) = default;
  PriorityQueue(PriorityQueue&& other) noexcept = default;
  PriorityQueue& operator=(const PriorityQueue& other) = default;
  PriorityQueue& operator=(PriorityQueue&& other) noexcept = default;

  void push(value_type x);
  void pop();
  [[nodiscard]] const value_type& top() const;
  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] size_type size() const noexcept;
  void clear() noexcept;
  void reserve(size_type n);
  [[nodiscard]] bool is_max_heap() const noexcept;

  bool operator==(const PriorityQueue& rhs) const noexcept;
  bool operator!=(const PriorityQueue& rhs) const noexcept;

  friend std::ostream& operator<<(std::ostream& os, const PriorityQueue& pq);

private:
  std::vector<value_type> heap_;
  bool is_max_heap_;

  void SiftUp(size_type idx);
  void SiftDown(size_type idx);
  [[nodiscard]] bool HasHigherPriority(value_type a, value_type b) const;
};

#endif  // PRIORITY_QUEUE_H_