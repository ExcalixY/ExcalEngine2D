#pragma once

#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <string_view>

#include "Component.hpp"

namespace ExcalCore {

struct Transform : Component {
  static constexpr std::string_view Name = "Transform";

  glm::vec2 pos;
  float rot;
  glm::vec2 scale;

  glm::mat3 matrix;
};

}
