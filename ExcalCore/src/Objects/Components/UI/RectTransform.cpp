#include "ExcalCore/Objects/Components/UI/RectTransform.hpp"

#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore {

const glm::vec2& RectTransform::GetAnchor() const {
  return m_Anchor;
}

void RectTransform::SetAnchor(const glm::vec2& anchor) {
  m_Anchor = anchor;
}

const glm::vec2& RectTransform::GetPivot() const {
  return m_Pivot;
}

void RectTransform::SetPivot(const glm::vec2& pivot) {
  m_Pivot = pivot;
}

const glm::vec2& RectTransform::GetPosition() const {
  return m_Position;
}

void RectTransform::SetPosition(const glm::vec2& position) {
  m_Position = position;
}

const glm::vec2& RectTransform::GetSize() const {
  return m_Size;
}

void RectTransform::SetSize(const glm::vec2& size) {
  m_Size = size;
}

const SDL_FRect& RectTransform::ComputeRect(const glm::vec2& screenSize) {
  glm::vec2 top_left = m_Anchor * screenSize + m_Position - m_Pivot * m_Size;
  m_LastRect = {top_left.x, top_left.y, m_Size.x, m_Size.y};
  return m_LastRect;
}

const SDL_FRect& RectTransform::ComputeRect(Renderer2D& renderer) {
  return ComputeRect(GetScreenSize(renderer));
}

const SDL_FRect& RectTransform::GetLastRect() const {
  return m_LastRect;
}

bool RectTransform::Contains(const glm::vec2& point) const {
  SDL_FPoint p = {point.x, point.y};
  return SDL_PointInRectFloat(&p, &m_LastRect);
}

glm::vec2 RectTransform::GetScreenSize(Renderer2D& renderer) {
  SDL_Renderer* raw = renderer.GetRendererRaw();

  int w = 0;
  int h = 0;
  SDL_RendererLogicalPresentation mode = SDL_LOGICAL_PRESENTATION_DISABLED;
  SDL_GetRenderLogicalPresentation(raw, &w, &h, &mode);

  if (mode == SDL_LOGICAL_PRESENTATION_DISABLED) {
    SDL_GetCurrentRenderOutputSize(raw, &w, &h);
  }

  return {static_cast<float>(w), static_cast<float>(h)};
}

}
