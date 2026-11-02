#include "ExcalCore/Assets/FontAsset.hpp"

#include <imgui.h>

#include <memory>
#include <string>

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

FontAsset::FontAsset(ImFont* font, float fontSize)
    : m_Font(font), m_FontSize(fontSize) { }

FontAsset::~FontAsset() {
  m_Font = nullptr;
}

std::shared_ptr<FontAsset> FontAsset::Load(
  const std::string& path, float pointSize
) {
  ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str());

  if (!font) {
    Debug::LogError(Debug::CATEGORY_ASSETS, "Failed to load Font Asset.");
    return nullptr;
  }

  return std::make_shared<FontAsset>(font, pointSize);
}

}
