#pragma once

#include <SDL3/SDL.h>

#include <glm/vec2.hpp>

namespace ExcalCore {

struct UIElement { };

enum class PointerEventType { Move, Down, Up };

struct PointerEvent {
  PointerEventType type;
  glm::vec2 position;
  Uint8 button = SDL_BUTTON_LEFT;
};

struct IPointerHandler {
  virtual ~IPointerHandler() = default;

  virtual bool OnPointerEvent(const PointerEvent& event) = 0;
};

}
