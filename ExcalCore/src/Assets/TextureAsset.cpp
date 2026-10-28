#include "ExcalCore/Assets/TextureAsset.hpp"

#include <memory>

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

TextureAsset::TextureAsset(SDL_Surface* surface) : m_Surface(surface) { }

TextureAsset::~TextureAsset() {
  SDL_DestroySurface(m_Surface);
}

std::shared_ptr<TextureAsset> TextureAsset::Load(const std::string& path) {
  SDL_Surface* m_Surface = SDL_LoadSurface(path.c_str());

  if (!m_Surface) {
    Debug::LogError(
      Debug::CATEGORY_ASSETS,
      "Failed to load Texture in path \"{}\" : {}",
      path,
      SDL_GetError()
    );

    return nullptr;
  }

  return std::make_shared<TextureAsset>(m_Surface);
}

glm::vec2 TextureAsset::GetSize() const {
  return {m_Surface->w, m_Surface->h};
}

SDL_Surface* TextureAsset::GetSurface() const {
  return m_Surface;
}

}
