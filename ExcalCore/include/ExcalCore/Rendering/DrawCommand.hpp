#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>

#include <optional>

#include "ExcalCore/Assets/TextureAsset.hpp"

namespace ExcalCore {

struct DrawCommand {
  TextureAsset* texture = nullptr;
  std::optional<SDL_FRect> source;
  SDL_FRect destination = {0, 0, 0, 0};
  double rotation = 0.0;
  std::optional<SDL_FPoint> pivot;
  SDL_FlipMode flip = SDL_FLIP_NONE;
  SDL_FColor color = {1.0f, 1.0f, 1.0f, 1.0f};
  int layer = 0;
};

}
