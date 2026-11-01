#pragma once

#include <SDL3_ttf/SDL_ttf.h>

#include <memory>
#include <string>

#include "Asset.hpp"

namespace ExcalCore {

class FontAsset : public Asset {
public:
  static constexpr std::string Name = "FontAsset";

  FontAsset(const FontAsset&) = delete;
  FontAsset& operator=(FontAsset&&) = delete;
  FontAsset(FontAsset&&) = delete;
  FontAsset& operator=(const FontAsset&) = delete;

  explicit FontAsset(TTF_Font* font, float fontSize)
      : m_Font(font), m_FontSize(fontSize) { }

  ~FontAsset() {
    TTF_CloseFont(m_Font);
    m_Font = nullptr;
  }

  [[nodiscard]]
  static std::shared_ptr<FontAsset> Load(
    const std::string& path, float pointSize = 16.0f
  );

private:
  TTF_Font* m_Font;
  float m_FontSize;
};

}
