#pragma once

#include <concepts>
#include <functional>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/Event.hpp"
#include "ExcalCore/Events/KeyEvents.hpp"
#include "ExcalCore/Events/MouseEvents.hpp"
#include "ExcalCore/Events/WindowFocusEvent.hpp"
#include "ExcalCore/Events/WindowResizeEvent.hpp"
#include "ExcalCore/Events/WindowShouldCloseEvent.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

class EventSystem final {
public:
  explicit EventSystem(ApplicationSpec& spec) {
    Debug::Log("Initializing Event System...");
  }

  ~EventSystem() {
    Debug::Log("Destroying Event System...");
  }

  template <typename TEvent, typename... TSystems>
    requires std::derived_from<TEvent, Event>
  void DispatchEvent(const TEvent& event, TSystems&... systems) {
    (
      [&] {
        if constexpr (requires() { systems.OnEvent(event); }) {
          systems.OnEvent(event);
        }
      }(),
      ...);
  }

  template <typename TCallback>
  void PollEvents(TCallback&& callback) {
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

  template <typename TEvent>
    requires std::derived_from<TEvent, Event>
  void Subscribe(std::function<void(const TEvent&)>) { }

private:
};

}
