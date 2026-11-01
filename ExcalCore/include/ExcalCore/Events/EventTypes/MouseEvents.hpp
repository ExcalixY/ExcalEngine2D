#pragma once

#include <SDL3/SDL_stdinc.h>

#include <glm/ext/vector_float2.hpp>
#include <string_view>

#include "ExcalCore/Events/Event.hpp"

namespace ExcalCore {

struct MouseMovedEvent : Event {
  static constexpr std::string_view Name = "MouseMoved";

  glm::vec2 position = {0, 0};
  glm::vec2 delta = {0, 0};
};

struct MouseButtonPressedEvent : Event {
  static constexpr std::string_view Name = "MouseButtonPressed";

  Uint8 button = 0;
  glm::vec2 position = {0, 0};
  Uint8 clicks = 1;
};

struct MouseButtonReleasedEvent : Event {
  static constexpr std::string_view Name = "MouseButtonReleased";

  Uint8 button = 0;
  glm::vec2 position = {0, 0};
};

struct MouseScrolledEvent : Event {
  static constexpr std::string_view Name = "MouseScrolled";

  glm::vec2 delta = {0, 0};
  glm::vec2 position = {0, 0};
};

}
