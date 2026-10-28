#pragma once

#include <SDL3/SDL.h>

#include <bitset>
#include <concepts>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/KeyEvents.hpp"
#include "ExcalCore/Events/MouseEvents.hpp"
#include "ExcalCore/Events/WindowFocusEvent.hpp"
#include "ExcalCore/Input/InputBindings.hpp"

namespace ExcalCore {

class InputSystem final {
public:
  explicit InputSystem(ApplicationSpec& spec);
  ~InputSystem();

  void BeginFrame();

  void OnEvent(const KeyPressedEvent& e);
  void OnEvent(const KeyReleasedEvent& e);
  void OnEvent(const MouseMovedEvent& e);
  void OnEvent(const MouseButtonPressedEvent& e);
  void OnEvent(const MouseButtonReleasedEvent& e);
  void OnEvent(const MouseScrolledEvent& e);
  void OnEvent(const WindowFocusEvent& e);

  void BindInput(std::string name, InputBinding binding);

  template <InputBindingType TBinding, typename... TArgs>
    requires std::constructible_from<TBinding, TArgs...>
  void BindInput(std::string name, TArgs&&... args) {
    BindInput(
      std::move(name),
      InputBinding(std::in_place_type<TBinding>, std::forward<TArgs>(args)...)
    );
  }

  template <InputBindingType TBinding>
  void BindInput(std::string name, std::type_identity_t<TBinding> binding) {
    BindInput(std::move(name), InputBinding(std::move(binding)));
  }
  void UnbindInput(std::string_view name);
  bool HasBinding(std::string_view name) const;

  ButtonState GetButton(std::string_view name) const;
  float GetAxis(std::string_view name) const;
  glm::vec2 GetAxis2D(std::string_view name) const;
  glm::vec3 GetAxis3D(std::string_view name) const;

  bool IsKeyDown(KeyCode key) const;
  bool IsKeyPressed(KeyCode key) const;
  bool IsKeyReleased(KeyCode key) const;

  bool IsMouseButtonDown(MouseButton button) const;
  bool IsMouseButtonPressed(MouseButton button) const;
  bool IsMouseButtonReleased(MouseButton button) const;

  const glm::vec2& GetMousePosition() const;
  const glm::vec2& GetMouseDelta() const;
  const glm::vec2& GetMouseScroll() const;

private:
  struct StringHash {
    using is_transparent = void;

    size_t operator()(std::string_view value) const {
      return std::hash<std::string_view>{}(value);
    }
  };

  template <typename TBinding>
  const TBinding* FindBinding(std::string_view name) const;

  ButtonState EvaluateSource(const ButtonSource& source) const;
  ButtonState EvaluateSources(const ButtonSources& sources) const;
  float EvaluateAxis(
    const ButtonSources& negative, const ButtonSources& positive
  ) const;

  static constexpr size_t MOUSE_BUTTON_COUNT = 8;

  std::bitset<SDL_SCANCODE_COUNT> m_KeysDown;
  std::bitset<SDL_SCANCODE_COUNT> m_KeysPressed;
  std::bitset<SDL_SCANCODE_COUNT> m_KeysReleased;

  std::bitset<MOUSE_BUTTON_COUNT> m_MouseDown;
  std::bitset<MOUSE_BUTTON_COUNT> m_MousePressed;
  std::bitset<MOUSE_BUTTON_COUNT> m_MouseReleased;

  glm::vec2 m_MousePosition = {0, 0};
  glm::vec2 m_MouseDelta = {0, 0};
  glm::vec2 m_MouseScroll = {0, 0};

  std::unordered_map<std::string, InputBinding, StringHash, std::equal_to<>>
    m_Bindings;
  mutable std::unordered_set<std::string, StringHash, std::equal_to<>>
    m_WarnedBindings;
};

}
