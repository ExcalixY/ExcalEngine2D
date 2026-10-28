#pragma once

#include <SDL3/SDL.h>

#include <glm/vec2.hpp>

#include "ExcalCore/Objects/Components/Component.hpp"
#include "UIElement.hpp"

namespace ExcalCore {

struct RectTransform : Component<RectTransform>, UIElement {
  static constexpr std::string_view Name = "RectTransform";

  const glm::vec2& GetAnchor() const;
  void SetAnchor(const glm::vec2& anchor);

  const glm::vec2& GetPivot() const;
  void SetPivot(const glm::vec2& pivot);

  const glm::vec2& GetPosition() const;
  void SetPosition(const glm::vec2& position);

  const glm::vec2& GetSize() const;
  void SetSize(const glm::vec2& size);

  const SDL_FRect& ComputeRect(const glm::vec2& screenSize);
  const SDL_FRect& ComputeRect(Renderer2D& renderer);

  const SDL_FRect& GetLastRect() const;
  bool Contains(const glm::vec2& point) const;

  static glm::vec2 GetScreenSize(Renderer2D& renderer);

private:
  glm::vec2 m_Anchor = {0.5f, 0.5f};
  glm::vec2 m_Pivot = {0.5f, 0.5f};
  glm::vec2 m_Position = {0, 0};
  glm::vec2 m_Size = {100, 100};

  SDL_FRect m_LastRect = {0, 0, 0, 0};
};

}
