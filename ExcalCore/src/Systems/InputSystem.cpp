#include "ExcalCore/Systems/InputSystem.hpp"

#include <utility>

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

InputSystem::InputSystem(ApplicationSpec& spec) {
  Debug::Log("Initializing Input System...");
}

InputSystem::~InputSystem() {
  Debug::Log("Destroying Input System...");
}

void InputSystem::BeginFrame() {
  m_KeysPressed.reset();
  m_KeysReleased.reset();

  m_MousePressed.reset();
  m_MouseReleased.reset();

  m_MouseDelta = {0, 0};
  m_MouseScroll = {0, 0};
}

void InputSystem::OnEvent(const KeyPressedEvent& e) {
  if (e.scancode >= SDL_SCANCODE_COUNT) {
    return;
  }

  if (!e.repeat) {
    m_KeysPressed.set(e.scancode);
  }
  m_KeysDown.set(e.scancode);
}

void InputSystem::OnEvent(const KeyReleasedEvent& e) {
  if (e.scancode >= SDL_SCANCODE_COUNT) {
    return;
  }

  m_KeysDown.reset(e.scancode);
  m_KeysReleased.set(e.scancode);
}

void InputSystem::OnEvent(const MouseMovedEvent& e) {
  m_MousePosition = e.position;
  m_MouseDelta += e.delta;
}

void InputSystem::OnEvent(const MouseButtonPressedEvent& e) {
  m_MousePosition = e.position;

  if (e.button >= MOUSE_BUTTON_COUNT) {
    return;
  }

  m_MouseDown.set(e.button);
  m_MousePressed.set(e.button);
}

void InputSystem::OnEvent(const MouseButtonReleasedEvent& e) {
  m_MousePosition = e.position;

  if (e.button >= MOUSE_BUTTON_COUNT) {
    return;
  }

  m_MouseDown.reset(e.button);
  m_MouseReleased.set(e.button);
}

void InputSystem::OnEvent(const MouseScrolledEvent& e) {
  m_MouseScroll += e.delta;
}

void InputSystem::OnEvent(const WindowFocusEvent& e) {
  if (e.focused) {
    return;
  }

  m_KeysReleased |= m_KeysDown;
  m_KeysDown.reset();

  m_MouseReleased |= m_MouseDown;
  m_MouseDown.reset();
}

void InputSystem::BindInput(std::string name, InputBinding binding) {
  m_WarnedBindings.erase(name);
  m_Bindings.insert_or_assign(std::move(name), std::move(binding));
}

void InputSystem::UnbindInput(std::string_view name) {
  if (auto it = m_Bindings.find(name); it != m_Bindings.end()) {
    m_Bindings.erase(it);
  }
}

bool InputSystem::HasBinding(std::string_view name) const {
  return m_Bindings.contains(name);
}

ButtonState InputSystem::GetButton(std::string_view name) const {
  const BindingButton* binding = FindBinding<BindingButton>(name);
  if (!binding) {
    return {};
  }

  return EvaluateSources(binding->sources);
}

float InputSystem::GetAxis(std::string_view name) const {
  const BindingAxis* binding = FindBinding<BindingAxis>(name);
  if (!binding) {
    return 0.f;
  }

  return EvaluateAxis(binding->negative, binding->positive);
}

glm::vec2 InputSystem::GetAxis2D(std::string_view name) const {
  const BindingAxis2D* binding = FindBinding<BindingAxis2D>(name);
  if (!binding) {
    return {0, 0};
  }

  if (
    const auto* composite = std::get_if<ButtonComposite2D>(&binding->source)
  ) {
    return {
      EvaluateAxis(composite->left, composite->right),
      EvaluateAxis(composite->up, composite->down),
    };
  }

  switch (std::get<MouseVector>(binding->source)) {
    case MouseVector::POSITION:
      return m_MousePosition;
    case MouseVector::DELTA:
      return m_MouseDelta;
    case MouseVector::SCROLL:
      return m_MouseScroll;
  }

  return {0, 0};
}

glm::vec3 InputSystem::GetAxis3D(std::string_view name) const {
  const BindingAxis3D* binding = FindBinding<BindingAxis3D>(name);
  if (!binding) {
    return {0, 0, 0};
  }

  const ButtonComposite3D& composite = binding->source;

  return {
    EvaluateAxis(composite.left, composite.right),
    EvaluateAxis(composite.up, composite.down),
    EvaluateAxis(composite.backward, composite.forward),
  };
}

bool InputSystem::IsKeyDown(KeyCode key) const {
  size_t index = static_cast<size_t>(key);
  return index < SDL_SCANCODE_COUNT && m_KeysDown.test(index);
}

bool InputSystem::IsKeyPressed(KeyCode key) const {
  size_t index = static_cast<size_t>(key);
  return index < SDL_SCANCODE_COUNT && m_KeysPressed.test(index);
}

bool InputSystem::IsKeyReleased(KeyCode key) const {
  size_t index = static_cast<size_t>(key);
  return index < SDL_SCANCODE_COUNT && m_KeysReleased.test(index);
}

bool InputSystem::IsMouseButtonDown(MouseButton button) const {
  size_t index = static_cast<size_t>(button);
  return index < MOUSE_BUTTON_COUNT && m_MouseDown.test(index);
}

bool InputSystem::IsMouseButtonPressed(MouseButton button) const {
  size_t index = static_cast<size_t>(button);
  return index < MOUSE_BUTTON_COUNT && m_MousePressed.test(index);
}

bool InputSystem::IsMouseButtonReleased(MouseButton button) const {
  size_t index = static_cast<size_t>(button);
  return index < MOUSE_BUTTON_COUNT && m_MouseReleased.test(index);
}

const glm::vec2& InputSystem::GetMousePosition() const {
  return m_MousePosition;
}

const glm::vec2& InputSystem::GetMouseDelta() const {
  return m_MouseDelta;
}

const glm::vec2& InputSystem::GetMouseScroll() const {
  return m_MouseScroll;
}

size_t InputSystem::StringHash::operator()(std::string_view value) const {
  return std::hash<std::string_view>{}(value);
}

template <typename TBinding>
const TBinding* InputSystem::FindBinding(std::string_view name) const {
  auto it = m_Bindings.find(name);

  if (it == m_Bindings.end()) {
    if (m_WarnedBindings.emplace(name).second) {
      Debug::LogWarning(
        Debug::CATEGORY_INPUT, "No input binding named '{}'", name
      );
    }
    return nullptr;
  }

  const TBinding* binding = std::get_if<TBinding>(&it->second);

  if (!binding && m_WarnedBindings.emplace(name).second) {
    Debug::LogWarning(
      Debug::CATEGORY_INPUT,
      "Input binding '{}' was queried as the wrong binding type",
      name
    );
  }

  return binding;
}

ButtonState InputSystem::EvaluateSource(const ButtonSource& source) const {
  if (const auto* key = std::get_if<KeySource>(&source)) {
    return {
      .down = IsKeyDown(key->key),
      .pressed = IsKeyPressed(key->key),
      .released = IsKeyReleased(key->key),
    };
  }

  if (const auto* mouse = std::get_if<MouseButtonSource>(&source)) {
    return {
      .down = IsMouseButtonDown(mouse->button),
      .pressed = IsMouseButtonPressed(mouse->button),
      .released = IsMouseButtonReleased(mouse->button),
    };
  }

  bool scrolled = false;

  switch (std::get<MouseScrollSource>(source).direction) {
    case ScrollDirection::UP:
      scrolled = m_MouseScroll.y > 0.f;
      break;
    case ScrollDirection::DOWN:
      scrolled = m_MouseScroll.y < 0.f;
      break;
    case ScrollDirection::LEFT:
      scrolled = m_MouseScroll.x < 0.f;
      break;
    case ScrollDirection::RIGHT:
      scrolled = m_MouseScroll.x > 0.f;
      break;
  }

  return {.down = scrolled, .pressed = scrolled, .released = false};
}

ButtonState InputSystem::EvaluateSources(const ButtonSources& sources) const {
  ButtonState state;

  for (const ButtonSource& source : sources) {
    ButtonState source_state = EvaluateSource(source);

    state.down |= source_state.down;
    state.pressed |= source_state.pressed;
    state.released |= source_state.released;
  }

  return state;
}

float InputSystem::EvaluateAxis(
  const ButtonSources& negative, const ButtonSources& positive
) const {
  float value = 0.f;

  if (EvaluateSources(negative).down) {
    value -= 1.f;
  }
  if (EvaluateSources(positive).down) {
    value += 1.f;
  }

  return value;
}

}
