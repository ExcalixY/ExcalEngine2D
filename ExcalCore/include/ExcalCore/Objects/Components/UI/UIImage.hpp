#pragma once

#include <SDL3/SDL.h>

#include <optional>

#include "ExcalCore/Objects/Components/Component.hpp"
#include "UIElement.hpp"

namespace ExcalCore {

struct RectTransform;

struct NineSlice {
  float left = 0.0f;
  float right = 0.0f;
  float top = 0.0f;
  float bottom = 0.0f;
  float scale = 1.0f;
};

struct UIImage : Component<UIImage>, IRenderable, UIElement {
  static constexpr std::string_view Name = "UIImage";

  UIImage() = default;
  explicit UIImage(SDL_Texture* texture);

  void OnRender(Renderer2D& renderer) override;
  int GetLayer() const override;
  void SetLayer(int layer);

  SDL_Texture* GetTexture() const;
  void SetTexture(SDL_Texture* texture);

  const std::optional<SDL_FRect>& GetSourceRect() const;
  void SetSourceRect(const SDL_FRect& rect);
  void ClearSourceRect();

  const std::optional<NineSlice>& GetNineSlice() const;
  void SetNineSlice(const NineSlice& slice);
  void ClearNineSlice();

  const SDL_FColor& GetColor() const;
  void SetColor(const SDL_FColor& color);

private:
  RectTransform* FindRectTransform();

  RectTransform* m_RectTransform = nullptr;
  SDL_Texture* m_Texture = nullptr;

  std::optional<SDL_FRect> m_SourceRect;
  std::optional<NineSlice> m_NineSlice;

  SDL_FColor m_Color = {1.0f, 1.0f, 1.0f, 1.0f};
  int m_Layer = 0;
};

}
