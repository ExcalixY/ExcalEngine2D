#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <functional>
#include <tuple>
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
  struct EventEntry {
    uint64_t id;
    int priority;
    std::function<bool(TEvent&)> callback;
  };

  uint64_t InsertCallback(std::function<bool(TEvent&)> fn, int priority = 0) {
    uint64_t id = 0;

    if (!m_FreeIndices.empty()) {
      id = m_FreeIndices.back();
      m_FreeIndices.pop_back();
    } else {
      id = m_Callbacks.size();
    }

    auto pos = std::ranges::upper_bound(
      m_Callbacks, priority, std::greater{}, &EventEntry::priority
    );
    m_Callbacks.emplace(pos, id, priority, std::move(fn));

    return id;
  }

  void RemoveCallback(uint64_t id) override {
    auto it = std::ranges::find_if(m_Callbacks, [id](const EventEntry& entry) {
      return entry.id == id;
    });

    if (it != m_Callbacks.end()) {
      m_Callbacks.erase(it);
      m_FreeIndices.push_back(id);
    }
  }

  void ExecuteCallbacks(TEvent& e) {
    for (auto& entry : m_Callbacks) {
      if (entry.callback(e)) {
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
  std::vector<EventEntry> m_Callbacks;
  std::vector<uint64_t> m_FreeIndices;

  std::vector<TEvent> m_EventQueue;
};

};
