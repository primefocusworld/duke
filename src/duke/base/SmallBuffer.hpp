#pragma once
#include "duke/base/Slice.hpp"
#include <algorithm>
#include <limits>
#include <utility>

#include <cstdlib>
#include <cstdint>
#include <cstring>

#define MAY_ALIAS __attribute__((__may_alias__))
/**
 * A buffer with small size optimization.
 *
 * If size is less or equal to SMALL_SIZE no allocation is performed.
 * Otherwise malloc/free is called to dynamically allocate the memory.
 * The structure is designed to be minimal yet efficient by enabling move
 * construction and assignment.
 */
template <size_t SMALL_SIZE_>
struct SmallBuffer {
  static constexpr size_t SMALL_SIZE = SMALL_SIZE_;
  static_assert(SMALL_SIZE >= sizeof(void *), "SmallData should at least be of pointer size, T>=8");

  SmallBuffer() : _size(0) { allocate(); }

  SmallBuffer(size_t size) : _size(size) { allocate(); }

  SmallBuffer(const void *ptr, size_t size) : _size(size) { allocateAndCopy(ptr); }

  SmallBuffer(MemorySlice slice) : SmallBuffer(slice.begin(), slice.size()) {}

  SmallBuffer(const SmallBuffer &other) : _size(other._size) { allocateAndCopy(other.begin()); }

  SmallBuffer &operator=(const SmallBuffer &other) {
    if (_size != other._size) {
      release();
      _size = other._size;
    }
    allocateAndCopy(other.begin());
    return *this;
  }

  SmallBuffer(SmallBuffer &&other) : _size(0) { stealContentFrom(std::move(other)); }

  SmallBuffer &operator=(SmallBuffer &&other) {
    release();
    stealContentFrom(std::move(other));
    return *this;
  }

  ~SmallBuffer() { release(); }

  inline const uint8_t *begin() const { return addressof(); }
  inline uint8_t *begin() { return addressof(); }

  inline const uint8_t *end() const { return begin() + _size; }
  inline uint8_t *end() { return begin() + _size; }

  void fill(const void *ptr) { memcpy(addressof(), ptr, _size); }

  inline size_t size() const { return _size; }

  inline bool empty() const { return _size == 0; }

  bool operator==(const SmallBuffer &other) const {
    return size() == other.size() && std::equal(begin(), end(), other.begin());
  }

 private:
  template <typename T>
  T &as(void *ptr) {
    return *reinterpret_cast<T *>(ptr);
  }
  template <typename T>
  T as(const void *ptr) const {
    return *reinterpret_cast<const T *>(ptr);
  }

  inline void setDynamicPtr(void *ptr) { as<void *>(_storage) = ptr; }

  inline uint8_t *addressof() { return isAllocated() ? as<uint8_t *>(_storage) : _storage; }

  inline const uint8_t *addressof() const { return isAllocated() ? as<const uint8_t *>(_storage) : _storage; }

  inline bool isAllocated() const { return _size > SMALL_SIZE; }

  inline void allocate() {
    if (isAllocated()) setDynamicPtr(malloc(_size));
  }

  inline void release() {
    if (isAllocated()) free(addressof());
    _size = 0;
  }

  void allocateAndCopy(const void *ptr) {
    allocate();
    fill(ptr);
  }

  void stealContentFrom(SmallBuffer &&other) {
    memcpy(_storage, other._storage, sizeof(_storage));
    _size = other._size;
    other._size = 0;
  }

  size_t _size;
  uint8_t _storage[SMALL_SIZE];
};
