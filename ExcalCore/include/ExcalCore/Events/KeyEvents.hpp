#pragma once

#include <SDL3/SDL.h>

#include <string_view>

#include "Event.hpp"

namespace ExcalCore {

struct KeyPressedEvent : Event {
  static constexpr std::string_view Name = "KeyPressed";

  SDL_Keycode key = SDLK_UNKNOWN;
  SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
  SDL_Keymod modifiers = SDL_KMOD_NONE;
  bool repeat = false;
};

struct KeyReleasedEvent : Event {
  static constexpr std::string_view Name = "KeyReleased";

  SDL_Keycode key = SDLK_UNKNOWN;
  SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
  SDL_Keymod modifiers = SDL_KMOD_NONE;
};

}
