#include "ExcalCore/Objects/Components/SpriteRenderer.hpp"

#include <glm/common.hpp>

#include "ExcalCore/Objects/Components/Transform.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore {

SpriteRenderer::SpriteRenderer(std::shared_ptr<TextureAsset> texture)
    : m_Texture(texture) { }

void SpriteRenderer::OnRender(Renderer2D& renderer) {
  if (!IsEnabled() || !m_Texture) {
    return;
  }

  Transform* transform = FindTransform();
  if (!transform) {
    return;
  }

  const glm::vec2& position = transform->GetPosition();
  const glm::vec2& scale = transform->GetScale();

  int flip = m_Flip;
  if (scale.x < 0.0f) {
    flip ^= SDL_FLIP_HORIZONTAL;
  }
  if (scale.y < 0.0f) {
    flip ^= SDL_FLIP_VERTICAL;
  }

  glm::vec2 size = GetSize() * glm::abs(scale);

  glm::vec2 pivot = m_Pivot;
  if (flip & SDL_FLIP_HORIZONTAL) {
    pivot.x = 1.0f - pivot.x;
  }
  if (flip & SDL_FLIP_VERTICAL) {
    pivot.y = 1.0f - pivot.y;
  }
  pivot *= size;

  SDL_FRect destination = {
    position.x - pivot.x, position.y - pivot.y, size.x, size.y
  };
  SDL_FPoint center = {pivot.x, pivot.y};

  renderer.SubmitDrawCommand({
    .texture = m_Texture,
    .source = m_SourceRect,
    .destination = destination,
    .rotation = transform->GetRotation(),
    .pivot = center,
    .flip = static_cast<SDL_FlipMode>(flip),
    .color = m_Color,
    .layer = m_Layer,
  });
}

int SpriteRenderer::GetLayer() const {
  return m_Layer;
}

void SpriteRenderer::SetLayer(int layer) {
  m_Layer = layer;
}

std::weak_ptr<TextureAsset> SpriteRenderer::GetTextureAsset() const {
  return m_Texture;
}

void SpriteRenderer::SetTexture(std::shared_ptr<TextureAsset> texture) {
  m_Texture = texture;
}

const std::optional<SDL_FRect>& SpriteRenderer::GetSourceRect() const {
  return m_SourceRect;
}

void SpriteRenderer::SetSourceRect(const SDL_FRect& rect) {
  m_SourceRect = rect;
}

void SpriteRenderer::ClearSourceRect() {
  m_SourceRect.reset();
}

glm::vec2 SpriteRenderer::GetSize() const {
  if (m_Size) {
    return *m_Size;
  }

  if (m_SourceRect) {
    return {m_SourceRect->w, m_SourceRect->h};
  }

  glm::vec2 size = {0, 0};

  if (m_Texture) {
    size = m_Texture->GetSize();
  }
  return size;
}

void SpriteRenderer::SetSize(const glm::vec2& size) {
  m_Size = size;
}

void SpriteRenderer::ResetSize() {
  m_Size.reset();
}

const glm::vec2& SpriteRenderer::GetPivot() const {
  return m_Pivot;
}

void SpriteRenderer::SetPivot(const glm::vec2& pivot) {
  m_Pivot = pivot;
}

const SDL_FColor& SpriteRenderer::GetColor() const {
  return m_Color;
}

void SpriteRenderer::SetColor(const SDL_FColor& color) {
  m_Color = color;
}

SDL_FlipMode SpriteRenderer::GetFlip() const {
  return m_Flip;
}

void SpriteRenderer::SetFlip(SDL_FlipMode flip) {
  m_Flip = flip;
}

Transform* SpriteRenderer::FindTransform() {
  if (!m_Transform && GetOwner()) {
    m_Transform = GetOwner()->GetComponent<Transform>();
  }
  return m_Transform;
}

}
