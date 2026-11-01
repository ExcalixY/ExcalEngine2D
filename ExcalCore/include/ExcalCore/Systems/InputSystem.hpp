#pragma once

#include <SDL3/SDL_scancode.h>

#include <bitset>
#include <concepts>
#include <cstddef>
#include <functional>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/EventTypes/KeyEvents.hpp"
#include "ExcalCore/Events/EventTypes/MouseEvents.hpp"
#include "ExcalCore/Events/EventTypes/WindowFocusEvent.hpp"
#include "ExcalCore/Input/InputBindings.hpp"
#include "ExcalCore/Input/KeyCode.hpp"
#include "ExcalCore/Input/MouseButton.hpp"

namespace ExcalCore {

class InputSystem final {
public:
  explicit InputSystem(ApplicationSpec& spec);
  ~InputSystem();

  InputSystem(const InputSystem&) = delete;
  InputSystem& operator=(const InputSystem&) = delete;

  [[nodiscard]]
  static const InputSystem* GetInstance();

  void BeginFrame();

  void OnEvent(KeyPressedEvent& e);
  void OnEvent(KeyReleasedEvent& e);
  void OnEvent(MouseMovedEvent& e);
  void OnEvent(MouseButtonPressedEvent& e);
  void OnEvent(MouseButtonReleasedEvent& e);
  void OnEvent(MouseScrolledEvent& e);
  void OnEvent(WindowFocusEvent& e);

  void BindInput(std::string name, InputBinding binding);

  template <InputBindingType TBinding, typename... TArgs>
    requires std::constructible_from<TBinding, TArgs...>
  void BindInput(std::string name, TArgs&&... args);

  template <InputBindingType TBinding>
  void BindInput(std::string name, std::type_identity_t<TBinding> binding);

  void UnbindInput(std::string_view name);
  [[nodiscard]]
  bool HasBinding(std::string_view name) const;

  [[nodiscard]]
  ButtonState GetButton(std::string_view name) const;
  [[nodiscard]]
  float GetAxis(std::string_view name) const;
  [[nodiscard]]
  glm::vec2 GetAxis2D(std::string_view name) const;
  [[nodiscard]]
  glm::vec3 GetAxis3D(std::string_view name) const;

  [[nodiscard]]
  bool IsKeyDown(KeyCode key) const;
  [[nodiscard]]
  bool IsKeyPressed(KeyCode key) const;
  [[nodiscard]]
  bool IsKeyReleased(KeyCode key) const;

  [[nodiscard]]
  bool IsMouseButtonDown(MouseButton button) const;
  [[nodiscard]]
  bool IsMouseButtonPressed(MouseButton button) const;
  [[nodiscard]]
  bool IsMouseButtonReleased(MouseButton button) const;

  [[nodiscard]]
  const glm::vec2& GetMousePosition() const;
  [[nodiscard]]
  const glm::vec2& GetMouseDelta() const;
  [[nodiscard]]
  const glm::vec2& GetMouseScroll() const;

private:
  static InputSystem* instance;

  struct StringHash {
    using is_transparent = void;

    size_t operator()(std::string_view value) const;
  };

  template <typename TBinding>
  [[nodiscard]]
  const TBinding* FindBinding(std::string_view name) const;

  [[nodiscard]]
  ButtonState EvaluateSource(const ButtonSource& source) const;
  [[nodiscard]]
  ButtonState EvaluateSources(const ButtonSources& sources) const;
  [[nodiscard]]
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

template <InputBindingType TBinding, typename... TArgs>
  requires std::constructible_from<TBinding, TArgs...>
void InputSystem::BindInput(std::string name, TArgs&&... args) {
  BindInput(
    std::move(name),
    InputBinding(std::in_place_type<TBinding>, std::forward<TArgs>(args)...)
  );
}

template <InputBindingType TBinding>
void InputSystem::BindInput(
  std::string name, std::type_identity_t<TBinding> binding
) {
  BindInput(std::move(name), InputBinding(std::move(binding)));
}

namespace Input {

[[nodiscard]]
bool HasBinding(std::string_view name);
[[nodiscard]]
ButtonState GetButton(std::string_view name);
[[nodiscard]]
float GetAxis(std::string_view name);
[[nodiscard]]
glm::vec2 GetAxis2D(std::string_view name);
[[nodiscard]]
glm::vec3 GetAxis3D(std::string_view name);
[[nodiscard]]
bool IsKeyDown(KeyCode key);
[[nodiscard]]
bool IsKeyPressed(KeyCode key);
[[nodiscard]]
bool IsKeyReleased(KeyCode key);
[[nodiscard]]
bool IsMouseButtonDown(MouseButton button);
[[nodiscard]]
bool IsMouseButtonPressed(MouseButton button);
[[nodiscard]]
bool IsMouseButtonReleased(MouseButton button);
[[nodiscard]]
const glm::vec2& GetMousePosition();
[[nodiscard]]
const glm::vec2& GetMouseDelta();
[[nodiscard]]
const glm::vec2& GetMouseScroll();

}

}
