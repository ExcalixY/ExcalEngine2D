#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>

#include <concepts>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/Event.hpp"
#include "ExcalCore/Events/EventContainer.hpp"
#include "ExcalCore/Events/EventHandle.hpp"
#include "ExcalCore/Events/EventTypes/KeyEvents.hpp"
#include "ExcalCore/Events/EventTypes/MouseEvents.hpp"
#include "ExcalCore/Events/EventTypes/WindowFocusEvent.hpp"
#include "ExcalCore/Events/EventTypes/WindowResizeEvent.hpp"
#include "ExcalCore/Events/EventTypes/WindowShouldCloseEvent.hpp"

namespace ExcalCore {

class EventSystem final {
public:
  explicit EventSystem(ApplicationSpec& spec);
  ~EventSystem();

  EventSystem(const EventSystem&) = delete;
  EventSystem& operator=(const EventSystem&) = delete;

  [[nodiscard]]
  static EventSystem* GetInstance();

  void UpdateEvents() {
    for (auto& [index, ptr] : m_Containers) {
      ptr->FlushQueue();
    }
  }

  template <typename TEvent, typename... TSystems>
    requires std::derived_from<TEvent, Event>
  void DispatchEvent(TEvent& event, TSystems&... systems);

  template <typename TCallback>
  void PollEvents(TCallback&& callback);

  template <typename TEvent, typename TFunc>
    requires(std::derived_from<TEvent, Event> && std::invocable<TFunc, TEvent&>)
  [[nodiscard]]
  EventHandle Subscribe(TFunc&& callback);

  void Unsubscribe(EventHandle& handle);

  template <typename TEvent, bool Immediate>
    requires std::derived_from<TEvent, Event>
  void Dispatch(TEvent& event);

private:
  template <typename TEvent>
    requires std::derived_from<TEvent, Event>
  EventContainer<TEvent>& GetContainer();

  static EventSystem* instance;

  std::unordered_map<std::type_index, std::unique_ptr<IEventContainer>>
    m_Containers;
};

template <typename TEvent, typename... TSystems>
  requires std::derived_from<TEvent, Event>
void EventSystem::DispatchEvent(TEvent& event, TSystems&... systems) {
  (
    [&] {
      if constexpr (requires() { systems.OnEvent(event); }) {
        systems.OnEvent(event);
      }
    }(),
    ...);

  Dispatch<TEvent, false>(event);
}

template <typename TCallback>
void EventSystem::PollEvents(TCallback&& callback) {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_EVENT_QUIT:
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        callback(WindowShouldCloseEvent{});
        break;

      case SDL_EVENT_WINDOW_RESIZED:
        callback(
          WindowResizeEvent{
            .width = e.window.data1,
            .height = e.window.data2,
          }
        );
        break;

      case SDL_EVENT_WINDOW_FOCUS_GAINED:
      case SDL_EVENT_WINDOW_FOCUS_LOST:
        callback(
          WindowFocusEvent{
            .focused = e.type == SDL_EVENT_WINDOW_FOCUS_GAINED,
          }
        );
        break;

      case SDL_EVENT_KEY_DOWN:
        callback(
          KeyPressedEvent{
            .key = e.key.key,
            .scancode = e.key.scancode,
            .modifiers = e.key.mod,
            .repeat = e.key.repeat,
          }
        );
        break;

      case SDL_EVENT_KEY_UP:
        callback(
          KeyReleasedEvent{
            .key = e.key.key,
            .scancode = e.key.scancode,
            .modifiers = e.key.mod,
          }
        );
        break;

      case SDL_EVENT_MOUSE_MOTION:
        callback(
          MouseMovedEvent{
            .position = {e.motion.x, e.motion.y},
            .delta = {e.motion.xrel, e.motion.yrel},
          }
        );
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        callback(
          MouseButtonPressedEvent{
            .button = e.button.button,
            .position = {e.button.x, e.button.y},
            .clicks = e.button.clicks,
          }
        );
        break;

      case SDL_EVENT_MOUSE_BUTTON_UP:
        callback(
          MouseButtonReleasedEvent{
            .button = e.button.button,
            .position = {e.button.x, e.button.y},
          }
        );
        break;

      case SDL_EVENT_MOUSE_WHEEL: {
        float direction =
          e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;

        callback(
          MouseScrolledEvent{
            .delta = {e.wheel.x * direction, e.wheel.y * direction},
            .position = {e.wheel.mouse_x, e.wheel.mouse_y},
          }
        );
        break;
      }

      default:
        break;
    }
  }
}

template <typename TEvent, typename TFunc>
  requires(std::derived_from<TEvent, Event> && std::invocable<TFunc, TEvent&>)
EventHandle EventSystem::Subscribe(TFunc&& callback) {
  return EventHandle(
    GetContainer<TEvent>().InsertCallback(
      std::function<bool(TEvent&)>(callback)
    ),
    typeid(TEvent)
  );
}

inline void EventSystem::Unsubscribe(EventHandle& handle) {
  auto it = m_Containers.find(handle.typeIndex);

  if (it != m_Containers.end()) {
    it->second.get()->RemoveCallback(handle.tagID);
  }
}

template <typename TEvent>
  requires std::derived_from<TEvent, Event>
EventContainer<TEvent>& EventSystem::GetContainer() {
  auto [it, inserted] =
    m_Containers.try_emplace(std::type_index(typeid(TEvent)), nullptr);

  if (inserted) {
    it->second = std::make_unique<EventContainer<TEvent>>();
  }

  return static_cast<EventContainer<TEvent>&>(*it->second);
}

template <typename TEvent, bool IsUrgent>
  requires std::derived_from<TEvent, Event>
void EventSystem::Dispatch(TEvent& event) {
  if constexpr (IsUrgent) {
    TEvent data = event;
    GetContainer<TEvent>().ExecuteCallbacks(data);
  } else {
    GetContainer<TEvent>().QueueEvent(event);
  }
}

namespace Events {

template <typename TEvent, typename TFunc>
  requires(std::derived_from<TEvent, Event> && std::invocable<TFunc, TEvent&>)
[[nodiscard]]
EventHandle Subscribe(TFunc&& callback) {
  return EventSystem::GetInstance()->Subscribe<TEvent>(
    std::forward<TFunc>(callback)
  );
}

inline void Unsubscribe(EventHandle& handle) {
  EventSystem::GetInstance()->Unsubscribe(handle);
}

template <typename TEvent, bool IsUrgent>
  requires std::derived_from<TEvent, Event>
void Dispatch(TEvent& event) {
  EventSystem::GetInstance()->Dispatch<TEvent, IsUrgent>(event);
}

}
}
