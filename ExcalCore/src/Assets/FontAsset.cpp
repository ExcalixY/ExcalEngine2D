#include "ExcalCore/Assets/FontAsset.hpp"

#include <memory>
#include <string>

#include "ExcalCore/Utils/Logging.hpp"
#include "SDL3_ttf/SDL_ttf.h"

namespace ExcalCore {

std::shared_ptr<FontAsset> FontAsset::Load(
  const std::string& path, float pointSize
) {
  TTF_Font* font = TTF_OpenFont(path.c_str(), pointSize);

  if (!font) {
    Debug::LogError(Debug::CATEGORY_ASSETS, "Failed to load Font Asset.");
    return nullptr;
  }

  return std::make_shared<FontAsset>(font, pointSize);
}

}
