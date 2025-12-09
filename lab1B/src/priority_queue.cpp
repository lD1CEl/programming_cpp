#include "priority_queue.h"
#include <algorithm>
#include <stdexcept>

namespace {

size_t Parent(const size_t i) { return (i - 1) / 2; }
size_t LeftChild(const size_t i) { return 2 * i + 1; }
size_t RightChild(const size_t i) { return 2 * i + 2; }

}

PriorityQueue::PriorityQueue() : is_max_heap_(true) {}

PriorityQueue::PriorityQueue(const bool max_heap) : is_max_heap_(max_heap) {}

PriorityQueue::PriorityQueue(const std::vector<value_type>& data,
                             const bool max_heap)
    : heap_(data), is_max_heap_(max_heap) {
  if (!heap_.empty()) {
    for (size_type i = heap_.size() / 2; i > 0; --i) {
      SiftDown(i - 1);
    }
  }
}

bool PriorityQueue::HasHigherPriority(const value_type a,
                                      const value_type b) const {
  return is_max_heap_ ? (a > b) : (a < b);
}

void PriorityQueue::SiftUp(size_type idx) {
  while (idx > 0) {
    const size_type parent_idx = Parent(idx);

    if (!HasHigherPriority(heap_[idx], heap_[parent_idx])) break;
    std::swap(heap_[parent_idx], heap_[idx]);
    idx = parent_idx;
  }
}

void PriorityQueue::SiftDown(size_type idx) {
  const size_type n = heap_.size();
  while (true) {
    size_type best = idx;
    const size_type left = LeftChild(idx);
    const size_type right = RightChild(idx);

    if (left < n && HasHigherPriority(heap_[left], heap_[best])) {
      best = left;
    }
    if (right < n && HasHigherPriority(heap_[right], heap_[best])) {
      best = right;
    }
    if (best == idx) break;
    std::swap(heap_[idx], heap_[best]);
    idx = best;
  }
}

void PriorityQueue::push(const value_type x) {
  heap_.push_back(x);
  SiftUp(heap_.size() - 1);
}

void PriorityQueue::pop() {
  if (empty()) {
    throw std::out_of_range("PriorityQueue is empty");
  }
  if (heap_.size() == 1) {
    heap_.pop_back();
    return;
  }
  heap_[0] = heap_.back();
  heap_.pop_back();
  SiftDown(0);
}

const PriorityQueue::value_type& PriorityQueue::top() const {
  if (empty()) {
    throw std::out_of_range("PriorityQueue is empty");
  }
  return heap_[0];
}

bool PriorityQueue::empty() const noexcept {
  return heap_.empty();
}

PriorityQueue::size_type PriorityQueue::size() const noexcept {
  return heap_.size();
}

void PriorityQueue::clear() noexcept {
  heap_.clear();
}

void PriorityQueue::reserve(const size_type n) {
  heap_.reserve(n);
}

bool PriorityQueue::is_max_heap() const noexcept {
  return is_max_heap_;
}

bool PriorityQueue::operator==(const PriorityQueue& rhs) const noexcept {
  return heap_ == rhs.heap_ && is_max_heap_ == rhs.is_max_heap_;
}

bool PriorityQueue::operator!=(const PriorityQueue& rhs) const noexcept {
  return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const PriorityQueue& pq) {
  os << "[";
  const auto& heap = pq.heap_;
  for (size_t i = 0; i < heap.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << heap[i];
  }
  os << "]";
  return os;
}
