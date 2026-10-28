#pragma once

#include <SDL3/SDL.h>

#include <glm/vec2.hpp>
#include <memory>
#include <optional>

#include "Component.hpp"
#include "ExcalCore/Assets/TextureAsset.hpp"

namespace ExcalCore {

struct Transform;

struct SpriteRenderer : Component<SpriteRenderer>, IRenderable {
  static constexpr std::string_view Name = "SpriteRenderer";

  SpriteRenderer() = default;
  explicit SpriteRenderer(std::shared_ptr<TextureAsset> texture);

  void OnRender(Renderer2D& renderer) override;
  int GetLayer() const override;
  void SetLayer(int layer);

  std::weak_ptr<TextureAsset> GetTextureAsset() const;
  void SetTexture(std::shared_ptr<TextureAsset> texture);

  const std::optional<SDL_FRect>& GetSourceRect() const;
  void SetSourceRect(const SDL_FRect& rect);
  void ClearSourceRect();

  glm::vec2 GetSize() const;
  void SetSize(const glm::vec2& size);
  void ResetSize();

  const glm::vec2& GetPivot() const;
  void SetPivot(const glm::vec2& pivot);

  const SDL_FColor& GetColor() const;
  void SetColor(const SDL_FColor& color);

  SDL_FlipMode GetFlip() const;
  void SetFlip(SDL_FlipMode flip);

private:
  Transform* FindTransform();

  Transform* m_Transform = nullptr;
  std::shared_ptr<TextureAsset> m_Texture = nullptr;

  std::optional<SDL_FRect> m_SourceRect;
  std::optional<glm::vec2> m_Size;

  glm::vec2 m_Pivot = {0.5f, 0.5f};
  SDL_FColor m_Color = {1.0f, 1.0f, 1.0f, 1.0f};
  SDL_FlipMode m_Flip = SDL_FLIP_NONE;
  int m_Layer = 0;
};

}
