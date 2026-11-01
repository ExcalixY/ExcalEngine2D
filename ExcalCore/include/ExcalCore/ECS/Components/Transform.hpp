#pragma once

#include <format>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <string_view>

#include "Component.hpp"
#include "ExcalCore/Utils/TypeFormats.hpp"  // IWYU pragma: keep

namespace ExcalCore {

struct Transform : Component {
  static constexpr std::string_view Name = "Transform";

  glm::vec2 pos = {0.0f, 0.0f};
  float rot = 0.0f;
  glm::vec2 scale = {1.0f, 1.0f};

  glm::mat3 localMatrix = {1.0f};
};

struct WorldTransform : Component {
  static constexpr std::string_view Name = "WorldTransform";

  glm::mat3 worldMatrix = {1.0f};
};

}

template <>
struct std::formatter<ExcalCore::Transform> {
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  auto format(
    const ExcalCore::Transform& transform, std::format_context& ctx
  ) const {
    return std::format_to(
      ctx.out(),
      "[pos: {}, rot: {}, scale: {}]",
      transform.pos,
      transform.rot,
      transform.scale
    );
  }
};
