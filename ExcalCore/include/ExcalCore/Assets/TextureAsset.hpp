#pragma once

#include <SDL3/SDL.h>

#include <glm/vec2.hpp>
#include <memory>
#include <string>

#include "Asset.hpp"

namespace ExcalCore {

class TextureAsset : public Asset {
public:
  explicit TextureAsset(SDL_Surface* surface);
  ~TextureAsset();

  static std::shared_ptr<TextureAsset> Load(const std::string& path);

  glm::vec2 GetSize() const;
  SDL_Surface* GetSurface() const;

private:
  SDL_Surface* m_Surface;
};

}
