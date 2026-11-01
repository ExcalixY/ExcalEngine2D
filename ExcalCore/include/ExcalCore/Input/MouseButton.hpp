#pragma once

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_stdinc.h>

namespace ExcalCore {

enum class MouseButton : Uint8 {
  LEFT = SDL_BUTTON_LEFT,
  MIDDLE = SDL_BUTTON_MIDDLE,
  RIGHT = SDL_BUTTON_RIGHT,
  X1 = SDL_BUTTON_X1,
  X2 = SDL_BUTTON_X2,
};

}
