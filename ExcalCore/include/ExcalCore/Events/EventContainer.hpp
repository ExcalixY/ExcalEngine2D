#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include "ExcalCore/Events/Event.hpp"

namespace ExcalCore {

class IEventContainer {
public:
  IEventContainer() = default;
  virtual ~IEventContainer() = default;
  virtual void RemoveCallback(uint64_t id) = 0;
  virtual void FlushQueue() = 0;
};

template <typename TEvent>
  requires(std::derived_from<TEvent, Event>)
class EventContainer : public IEventContainer {
public:
  uint64_t InsertCallback(std::function<bool(TEvent&)> fn) {
    uint64_t id = 0;

    if (!m_FreeIndices.empty()) {
      id = m_FreeIndices.back();
      m_FreeIndices.pop_back();
    } else {
      id = m_Callbacks.size();
    }

    m_Callbacks.emplace_back(id, fn);
    return id;
  }

  void RemoveCallback(uint64_t id) override {
    auto it = std::ranges::find_if(
      m_Callbacks,
      [id](const std::pair<uint64_t, std::function<bool(TEvent&)>>& s) {
        return s.first == id;
      }
    );

    if (it != m_Callbacks.end()) {
      std::swap(m_Callbacks.back(), *it);
      m_Callbacks.pop_back();

      m_FreeIndices.push_back(id);
    }
  }

  void ExecuteCallbacks(TEvent& e) {
    for (auto& c : m_Callbacks) {
      if (c.second(e)) {
        return;
      }
    }
  }

  void QueueEvent(TEvent& e) {
    m_EventQueue.push_back(e);
  }

  void FlushQueue() override {
    for (auto& e : m_EventQueue) {
      ExecuteCallbacks(e);
    }

    m_EventQueue.clear();
  }

private:
  std::vector<std::pair<uint64_t, std::function<bool(TEvent&)>>> m_Callbacks;
  std::vector<uint64_t> m_FreeIndices;

  std::vector<TEvent> m_EventQueue;
};

};
