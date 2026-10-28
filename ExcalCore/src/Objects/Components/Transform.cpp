#include "ExcalCore/Objects/Components/Transform.hpp"

#include <cmath>
#include <glm/trigonometric.hpp>

namespace ExcalCore {

const glm::vec2& Transform::GetPosition() const {
  return m_Position;
}

void Transform::SetPosition(const glm::vec2& position) {
  m_Position = position;
  m_Dirty = true;
}

void Transform::Translate(const glm::vec2& delta) {
  SetPosition(m_Position + delta);
}

float Transform::GetRotation() const {
  return m_Rotation;
}

void Transform::SetRotation(float degrees) {
  m_Rotation = std::fmod(degrees, 360.0f);
  if (m_Rotation < 0.0f) {
    m_Rotation += 360.0f;
  }
  m_Dirty = true;
}

void Transform::Rotate(float degrees) {
  SetRotation(m_Rotation + degrees);
}

const glm::vec2& Transform::GetScale() const {
  return m_Scale;
}

void Transform::SetScale(const glm::vec2& scale) {
  m_Scale = scale;
  m_Dirty = true;
}

glm::vec2 Transform::GetRight() const {
  float radians = glm::radians(m_Rotation);
  return {std::cos(radians), std::sin(radians)};
}

glm::vec2 Transform::GetUp() const {
  float radians = glm::radians(m_Rotation);
  return {std::sin(radians), -std::cos(radians)};
}

const glm::mat3& Transform::GetMatrix() const {
  if (m_Dirty) {
    float radians = glm::radians(m_Rotation);
    float c = std::cos(radians);
    float s = std::sin(radians);

    m_Matrix[0] = glm::vec3(c * m_Scale.x, s * m_Scale.x, 0.0f);
    m_Matrix[1] = glm::vec3(-s * m_Scale.y, c * m_Scale.y, 0.0f);
    m_Matrix[2] = glm::vec3(m_Position.x, m_Position.y, 1.0f);

    m_Dirty = false;
  }

  return m_Matrix;
}

glm::vec2 Transform::TransformPoint(const glm::vec2& localPoint) const {
  return glm::vec2(GetMatrix() * glm::vec3(localPoint, 1.0f));
}

}
