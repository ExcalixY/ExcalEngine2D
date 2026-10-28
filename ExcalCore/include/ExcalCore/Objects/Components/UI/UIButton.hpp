#pragma once

#include <SDL3/SDL.h>

#include <functional>

#include "ExcalCore/Objects/Components/Component.hpp"
#include "UIElement.hpp"

namespace ExcalCore {

struct RectTransform;
struct UIImage;

enum class ButtonState { Normal, Hovered, Pressed, Disabled };

struct ButtonColors {
  SDL_FColor normal = {1.0f, 1.0f, 1.0f, 1.0f};
  SDL_FColor hovered = {0.9f, 0.9f, 0.9f, 1.0f};
  SDL_FColor pressed = {0.7f, 0.7f, 0.7f, 1.0f};
  SDL_FColor disabled = {0.5f, 0.5f, 0.5f, 0.5f};
};

struct UIButton : Component<UIButton>, IStartable, IPointerHandler, UIElement {
  static constexpr std::string_view Name = "UIButton";

  void OnStart() override;
  void OnEnabled() override;
  void OnDisabled() override;

  bool OnPointerEvent(const PointerEvent& event) override;

  ButtonState GetState() const;

  bool IsInteractable() const;
  void SetInteractable(bool interactable);

  const ButtonColors& GetColors() const;
  void SetColors(const ButtonColors& colors);

  std::function<void()> onClick;

private:
  RectTransform* FindRectTransform();
  UIImage* FindImage();

  void ResetPointer();
  void UpdateState();
  void ApplyColor();

  RectTransform* m_RectTransform = nullptr;
  UIImage* m_Image = nullptr;

  ButtonColors m_Colors;
  ButtonState m_State = ButtonState::Normal;

  bool m_Interactable = true;
  bool m_Hovered = false;
  bool m_Pressed = false;
};

}
