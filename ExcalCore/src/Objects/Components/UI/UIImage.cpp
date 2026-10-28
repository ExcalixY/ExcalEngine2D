#include "ExcalCore/Objects/Components/UI/UIImage.hpp"

#include "ExcalCore/Objects/Components/UI/RectTransform.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore {

UIImage::UIImage(SDL_Texture* texture) : m_Texture(texture) { }

void UIImage::OnRender(Renderer2D& renderer) {
  if (!IsEnabled()) {
    return;
  }

  RectTransform* rect_transform = FindRectTransform();
  if (!rect_transform) {
    return;
  }

  SDL_Renderer* raw = renderer.GetRendererRaw();
  const SDL_FRect& rect = rect_transform->ComputeRect(renderer);

  if (!m_Texture) {
    SDL_SetRenderDrawBlendMode(raw, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColorFloat(
      raw, m_Color.r, m_Color.g, m_Color.b, m_Color.a
    );
    SDL_RenderFillRect(raw, &rect);
    return;
  }

  SDL_SetTextureColorModFloat(m_Texture, m_Color.r, m_Color.g, m_Color.b);
  SDL_SetTextureAlphaModFloat(m_Texture, m_Color.a);

  const SDL_FRect* source = m_SourceRect ? &*m_SourceRect : nullptr;

  if (m_NineSlice) {
    SDL_RenderTexture9Grid(
      raw,
      m_Texture,
      source,
      m_NineSlice->left,
      m_NineSlice->right,
      m_NineSlice->top,
      m_NineSlice->bottom,
      m_NineSlice->scale,
      &rect
    );
  } else {
    SDL_RenderTexture(raw, m_Texture, source, &rect);
  }
}

int UIImage::GetLayer() const {
  return m_Layer;
}

void UIImage::SetLayer(int layer) {
  m_Layer = layer;
}

SDL_Texture* UIImage::GetTexture() const {
  return m_Texture;
}

void UIImage::SetTexture(SDL_Texture* texture) {
  m_Texture = texture;
}

const std::optional<SDL_FRect>& UIImage::GetSourceRect() const {
  return m_SourceRect;
}

void UIImage::SetSourceRect(const SDL_FRect& rect) {
  m_SourceRect = rect;
}

void UIImage::ClearSourceRect() {
  m_SourceRect.reset();
}

const std::optional<NineSlice>& UIImage::GetNineSlice() const {
  return m_NineSlice;
}

void UIImage::SetNineSlice(const NineSlice& slice) {
  m_NineSlice = slice;
}

void UIImage::ClearNineSlice() {
  m_NineSlice.reset();
}

const SDL_FColor& UIImage::GetColor() const {
  return m_Color;
}

void UIImage::SetColor(const SDL_FColor& color) {
  m_Color = color;
}

RectTransform* UIImage::FindRectTransform() {
  if (!m_RectTransform && GetOwner()) {
    m_RectTransform = GetOwner()->GetComponent<RectTransform>();
  }
  return m_RectTransform;
}

}
