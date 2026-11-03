#pragma once

#include "UIComponent.hpp"
#include "glm/ext/vector_float2.hpp"

namespace ExcalCore {

struct UICanvas : UIComponent {
  glm::vec2 padding;
};

}
