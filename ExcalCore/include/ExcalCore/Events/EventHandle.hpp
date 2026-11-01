#pragma once

#include <cstdint>
#include <limits>
#include <typeindex>
#include <utility>

namespace ExcalCore {

class EventHandle final {
public:
  static constexpr uint64_t INVALID_TAG = std::numeric_limits<uint64_t>::max();

  explicit EventHandle(uint64_t id, std::type_index index)
      : tagID(id), typeIndex(index) { }
  EventHandle() = default;

  ~EventHandle() {
    Unsubscribe();
  }

  EventHandle(const EventHandle&) = delete;
  EventHandle& operator=(const EventHandle&) = delete;

  EventHandle(EventHandle&& other) noexcept
      : tagID(std::exchange(other.tagID, INVALID_TAG)),
        typeIndex(other.typeIndex) { }

  EventHandle& operator=(EventHandle&& other) noexcept {
    if (this != &other) {
      Unsubscribe();
      tagID = std::exchange(other.tagID, INVALID_TAG);
      typeIndex = other.typeIndex;
    }

    return *this;
  }

  void Unsubscribe();

private:
  friend class EventSystem;

  uint64_t tagID = INVALID_TAG;
  std::type_index typeIndex = typeid(void);
};

}
