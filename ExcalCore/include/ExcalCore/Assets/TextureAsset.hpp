#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <string>

#include "Asset.hpp"

namespace ExcalCore {

class TextureAsset : public Asset {
public:
  static constexpr std::string Name = "TextureAsset";

  TextureAsset(TextureAsset&&) = delete;
  TextureAsset& operator=(TextureAsset&&) = delete;
  TextureAsset(const TextureAsset&) = delete;
  TextureAsset& operator=(const TextureAsset&) = delete;

  explicit TextureAsset(SDL_Surface* surface);
  ~TextureAsset();

  [[nodiscard]]
  static std::shared_ptr<TextureAsset> Load(const std::string& path);

  [[nodiscard]]
  glm::vec2 GetSize() const;
  [[nodiscard]]
  SDL_Surface* GetSurface() const;

  [[nodiscard]]
  SDL_Texture* GetTexture() const;

private:
  friend class Renderer2D;
  void SetTexture(SDL_Texture* texture);

  SDL_Surface* m_Surface;
  SDL_Texture* m_Texture = nullptr;
};

}
