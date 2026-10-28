#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <glm/vec2.hpp>
#include <string>

#include "ExcalCore/Objects/Components/Component.hpp"
#include "UIElement.hpp"

namespace ExcalCore {

struct RectTransform;

enum class TextAlign { Start, Center, End };

struct UIText : Component<UIText>, IRenderable, UIElement {
  static constexpr std::string_view Name = "UIText";

  UIText() = default;
  UIText(TTF_Font* font, std::string text);
  ~UIText() override;

  UIText(const UIText&) = delete;
  UIText& operator=(const UIText&) = delete;

  void OnRender(Renderer2D& renderer) override;
  int GetLayer() const override;
  void SetLayer(int layer);

  TTF_Font* GetFont() const;
  void SetFont(TTF_Font* font);

  const std::string& GetText() const;
  void SetText(std::string text);

  const SDL_FColor& GetColor() const;
  void SetColor(const SDL_FColor& color);

  TextAlign GetHorizontalAlign() const;
  void SetHorizontalAlign(TextAlign align);

  TextAlign GetVerticalAlign() const;
  void SetVerticalAlign(TextAlign align);

  bool IsWrapping() const;
  void SetWrapping(bool wrap);

  void Refresh();

private:
  RectTransform* FindRectTransform();
  void RebuildTexture(SDL_Renderer* renderer, float wrapWidth);

  RectTransform* m_RectTransform = nullptr;
  TTF_Font* m_Font = nullptr;
  std::string m_Text;

  SDL_FColor m_Color = {1.0f, 1.0f, 1.0f, 1.0f};
  TextAlign m_HorizontalAlign = TextAlign::Center;
  TextAlign m_VerticalAlign = TextAlign::Center;
  bool m_Wrap = false;
  int m_Layer = 0;

  SDL_Texture* m_Texture = nullptr;
  glm::vec2 m_TextureSize = {0, 0};
  float m_BuiltWrapWidth = 0.0f;
  bool m_Dirty = true;
};

}
