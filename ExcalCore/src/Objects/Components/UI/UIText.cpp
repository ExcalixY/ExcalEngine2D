#include "ExcalCore/Objects/Components/UI/UIText.hpp"

#include <cmath>

#include "ExcalCore/Objects/Components/UI/RectTransform.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

namespace {

float Align(TextAlign align, float start, float available, float size) {
  switch (align) {
    case TextAlign::Start:
      return start;
    case TextAlign::Center:
      return start + (available - size) * 0.5f;
    case TextAlign::End:
      return start + available - size;
  }
  return start;
}

}

UIText::UIText(TTF_Font* font, std::string text)
    : m_Font(font), m_Text(std::move(text)) { }

UIText::~UIText() {
  SDL_DestroyTexture(m_Texture);
}

void UIText::OnRender(Renderer2D& renderer) {
  if (!IsEnabled()) {
    return;
  }

  RectTransform* rect_transform = FindRectTransform();
  if (!rect_transform) {
    return;
  }

  SDL_Renderer* raw = renderer.GetRendererRaw();
  const SDL_FRect& rect = rect_transform->ComputeRect(renderer);

  float wrap_width = m_Wrap ? rect.w : 0.0f;
  if (m_Dirty || wrap_width != m_BuiltWrapWidth) {
    RebuildTexture(raw, wrap_width);
  }

  if (!m_Texture) {
    return;
  }

  SDL_FRect destination = {
    std::round(Align(m_HorizontalAlign, rect.x, rect.w, m_TextureSize.x)),
    std::round(Align(m_VerticalAlign, rect.y, rect.h, m_TextureSize.y)),
    m_TextureSize.x,
    m_TextureSize.y
  };

  SDL_SetTextureColorModFloat(m_Texture, m_Color.r, m_Color.g, m_Color.b);
  SDL_SetTextureAlphaModFloat(m_Texture, m_Color.a);
  SDL_RenderTexture(raw, m_Texture, nullptr, &destination);
}

int UIText::GetLayer() const {
  return m_Layer;
}

void UIText::SetLayer(int layer) {
  m_Layer = layer;
}

TTF_Font* UIText::GetFont() const {
  return m_Font;
}

void UIText::SetFont(TTF_Font* font) {
  m_Font = font;
  m_Dirty = true;
}

const std::string& UIText::GetText() const {
  return m_Text;
}

void UIText::SetText(std::string text) {
  if (text == m_Text) {
    return;
  }

  m_Text = std::move(text);
  m_Dirty = true;
}

const SDL_FColor& UIText::GetColor() const {
  return m_Color;
}

void UIText::SetColor(const SDL_FColor& color) {
  m_Color = color;
}

TextAlign UIText::GetHorizontalAlign() const {
  return m_HorizontalAlign;
}

void UIText::SetHorizontalAlign(TextAlign align) {
  m_HorizontalAlign = align;
}

TextAlign UIText::GetVerticalAlign() const {
  return m_VerticalAlign;
}

void UIText::SetVerticalAlign(TextAlign align) {
  m_VerticalAlign = align;
}

bool UIText::IsWrapping() const {
  return m_Wrap;
}

void UIText::SetWrapping(bool wrap) {
  m_Wrap = wrap;
}

void UIText::Refresh() {
  m_Dirty = true;
}

RectTransform* UIText::FindRectTransform() {
  if (!m_RectTransform && GetOwner()) {
    m_RectTransform = GetOwner()->GetComponent<RectTransform>();
  }
  return m_RectTransform;
}

void UIText::RebuildTexture(SDL_Renderer* renderer, float wrapWidth) {
  SDL_DestroyTexture(m_Texture);
  m_Texture = nullptr;
  m_TextureSize = {0, 0};
  m_BuiltWrapWidth = wrapWidth;
  m_Dirty = false;

  if (!m_Font || m_Text.empty()) {
    return;
  }

  SDL_Color white = {255, 255, 255, 255};
  SDL_Surface* surface =
    wrapWidth > 0.0f
      ? TTF_RenderText_Blended_Wrapped(
          m_Font,
          m_Text.c_str(),
          m_Text.size(),
          white,
          static_cast<int>(wrapWidth)
        )
      : TTF_RenderText_Blended(m_Font, m_Text.c_str(), m_Text.size(), white);

  if (!surface) {
    Debug::LogError(
      Debug::CATEGORY_RENDER,
      "Couldn't render text '{}': {}",
      m_Text,
      SDL_GetError()
    );
    return;
  }

  m_Texture = SDL_CreateTextureFromSurface(renderer, surface);
  m_TextureSize = {
    static_cast<float>(surface->w), static_cast<float>(surface->h)
  };
  SDL_DestroySurface(surface);

  if (!m_Texture) {
    Debug::LogError(
      Debug::CATEGORY_RENDER, "Couldn't create text texture: {}", SDL_GetError()
    );
  }
}

}
