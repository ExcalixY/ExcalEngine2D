#pragma once

#include <string_view>

#include "ExcalCore/ECS/Components/UI/UIComponent.hpp"
#include "ExcalCore/ECS/ECSRef.hpp"
#include "glm/ext/vector_float2.hpp"

namespace ExcalCore {

struct UIComputedRect : UIComponent {
  static constexpr std::string_view Name = "UIComputedRect";

  glm::vec2 position = {0.0f, 0.0f};  // Local Position of the Rect
  glm::vec2 size = {0.0f, 0.0f};      // Local Size of the Rect

  glm::vec2 worldPosition = {0.0f, 0.0f};  // World Position of the Rect
};

struct UIRectTransform : UIComponent {
  static constexpr std::string_view Name = "UIRectTransform";

  glm::vec2 anchor = {0.0f, 0.0f};  // Anchor Position relative to local size
  glm::vec2 anchorOffset = {0.0f, 0.0f};  // Anchor Offset relative to world

  glm::vec2 pivot = {0.5f, 0.5f};  // Pivot position relative to local size
  glm::vec2 pivotOffset = {0.0f, 0.0f};  // Pivot offset relative to world

  ECSRef<UIComputedRect> rect;  // Ref to computed rect
};

}
