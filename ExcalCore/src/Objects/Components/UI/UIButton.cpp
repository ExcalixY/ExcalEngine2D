#include "ExcalCore/Objects/Components/UI/UIButton.hpp"

#include "ExcalCore/Objects/Components/UI/RectTransform.hpp"
#include "ExcalCore/Objects/Components/UI/UIImage.hpp"
#include "ExcalCore/Objects/GameObject.hpp"

namespace ExcalCore {

void UIButton::OnStart() {
  UpdateState();
  ApplyColor();
}

void UIButton::OnEnabled() {
  UpdateState();
}

void UIButton::OnDisabled() {
  ResetPointer();
  UpdateState();
}

bool UIButton::OnPointerEvent(const PointerEvent& event) {
  if (!IsEnabled() || !m_Interactable) {
    return false;
  }

  RectTransform* rect_transform = FindRectTransform();
  if (!rect_transform) {
    return false;
  }

  bool inside = rect_transform->Contains(event.position);

  switch (event.type) {
    case PointerEventType::Move:
      m_Hovered = inside;
      UpdateState();
      return false;

    case PointerEventType::Down:
      if (event.button != SDL_BUTTON_LEFT || !inside) {
        return false;
      }
      m_Pressed = true;
      UpdateState();
      return true;

    case PointerEventType::Up:
      if (event.button != SDL_BUTTON_LEFT || !m_Pressed) {
        return false;
      }
      m_Pressed = false;
      m_Hovered = inside;
      UpdateState();

      if (inside && onClick) {
        onClick();
      }
      return true;
  }

  return false;
}

ButtonState UIButton::GetState() const {
  return m_State;
}

bool UIButton::IsInteractable() const {
  return m_Interactable;
}

void UIButton::SetInteractable(bool interactable) {
  m_Interactable = interactable;

  if (!m_Interactable) {
    ResetPointer();
  }
  UpdateState();
}

const ButtonColors& UIButton::GetColors() const {
  return m_Colors;
}

void UIButton::SetColors(const ButtonColors& colors) {
  m_Colors = colors;
  ApplyColor();
}

RectTransform* UIButton::FindRectTransform() {
  if (!m_RectTransform && GetOwner()) {
    m_RectTransform = GetOwner()->GetComponent<RectTransform>();
  }
  return m_RectTransform;
}

UIImage* UIButton::FindImage() {
  if (!m_Image && GetOwner()) {
    m_Image = GetOwner()->GetComponent<UIImage>();
  }
  return m_Image;
}

void UIButton::ResetPointer() {
  m_Hovered = false;
  m_Pressed = false;
}

void UIButton::UpdateState() {
  ButtonState state = ButtonState::Normal;

  if (!IsEnabled() || !m_Interactable) {
    state = ButtonState::Disabled;
  } else if (m_Pressed && m_Hovered) {
    state = ButtonState::Pressed;
  } else if (m_Hovered) {
    state = ButtonState::Hovered;
  }

  if (state != m_State) {
    m_State = state;
    ApplyColor();
  }
}

void UIButton::ApplyColor() {
  UIImage* image = FindImage();
  if (!image) {
    return;
  }

  switch (m_State) {
    case ButtonState::Normal:
      image->SetColor(m_Colors.normal);
      break;
    case ButtonState::Hovered:
      image->SetColor(m_Colors.hovered);
      break;
    case ButtonState::Pressed:
      image->SetColor(m_Colors.pressed);
      break;
    case ButtonState::Disabled:
      image->SetColor(m_Colors.disabled);
      break;
  }
}

}
