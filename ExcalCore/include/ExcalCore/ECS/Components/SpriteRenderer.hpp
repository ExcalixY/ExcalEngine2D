#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>

#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>

#include "ExcalCore/Assets/TextureAsset.hpp"
#include "ExcalCore/ECS/Components/Component.hpp"

namespace ExcalCore {

struct SpriteRenderer : Component {
  std::shared_ptr<TextureAsset> texture = nullptr;
  std::optional<SDL_FRect> source;  // set by the animator for sprite sheets
  glm::vec2 pivot{0.5f, 0.5f};      // normalised, centre by default
  SDL_FlipMode flip = SDL_FLIP_NONE;
  SDL_FColor color = {1.f, 1.f, 1.f, 1.f};
  int layer = 0;  // later: layer + order for the sort key
};

}
