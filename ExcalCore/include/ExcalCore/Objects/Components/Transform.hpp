#pragma once

#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

#include "Component.hpp"

namespace ExcalCore {

struct Transform : Component<Transform> {
  static constexpr std::string_view Name = "Transform";

  const glm::vec2& GetPosition() const;
  void SetPosition(const glm::vec2& position);
  void Translate(const glm::vec2& delta);

  float GetRotation() const;
  void SetRotation(float degrees);
  void Rotate(float degrees);

  const glm::vec2& GetScale() const;
  void SetScale(const glm::vec2& scale);

  glm::vec2 GetRight() const;
  glm::vec2 GetUp() const;

  const glm::mat3& GetMatrix() const;
  glm::vec2 TransformPoint(const glm::vec2& localPoint) const;

private:
  glm::vec2 m_Position = {0, 0};
  glm::vec2 m_Scale = {1, 1};
  float m_Rotation = 0.0f;

  mutable glm::mat3 m_Matrix = glm::mat3(1.0f);
  mutable bool m_Dirty = true;
};

}
