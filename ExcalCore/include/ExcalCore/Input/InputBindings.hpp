#pragma once

#include <SDL3/SDL.h>

#include <concepts>
#include <initializer_list>
#include <utility>
#include <variant>
#include <vector>

#include "KeyCode.hpp"
#include "MouseButton.hpp"

namespace ExcalCore {

enum class ScrollDirection {
  UP,
  DOWN,
  LEFT,
  RIGHT,
};

enum class MouseVector {
  POSITION,
  DELTA,
  SCROLL,
};

struct KeySource {
  KeySource() = default;
  KeySource(KeyCode key) : key(key) { }

  KeyCode key = KeyCode::UNKNOWN;
};

struct MouseButtonSource {
  MouseButtonSource() = default;
  MouseButtonSource(MouseButton button) : button(button) { }

  MouseButton button = MouseButton::LEFT;
};

struct MouseScrollSource {
  MouseScrollSource() = default;
  MouseScrollSource(ScrollDirection direction) : direction(direction) { }

  ScrollDirection direction = ScrollDirection::UP;
};

using ButtonSource =
  std::variant<KeySource, MouseButtonSource, MouseScrollSource>;
using ButtonSources = std::vector<ButtonSource>;

struct ButtonState {
  bool down = false;
  bool pressed = false;
  bool released = false;
};

struct ButtonComposite2D {
  ButtonSources up;
  ButtonSources down;
  ButtonSources left;
  ButtonSources right;
};

struct ButtonComposite3D {
  ButtonSources up;
  ButtonSources down;
  ButtonSources left;
  ButtonSources right;
  ButtonSources forward;
  ButtonSources backward;
};

struct BindingButton {
  BindingButton() = default;

  template <typename... TSources>
    requires(sizeof...(TSources) > 0
             && (std::constructible_from<ButtonSource, TSources> && ...))
  explicit BindingButton(TSources... sources)
      : sources{ButtonSource(sources)...} { }

  BindingButton(std::initializer_list<ButtonSource> sources)
      : sources(sources) { }

  explicit BindingButton(ButtonSources sources) : sources(std::move(sources)) { }

  ButtonSources sources;
};

struct BindingAxis {
  BindingAxis() = default;

  BindingAxis(ButtonSource negative, ButtonSource positive)
      : negative{negative}, positive{positive} { }

  BindingAxis(ButtonSources negative, ButtonSources positive)
      : negative(std::move(negative)), positive(std::move(positive)) { }

  ButtonSources negative;
  ButtonSources positive;
};

struct BindingAxis2D {
  BindingAxis2D() = default;

  BindingAxis2D(
    ButtonSource up, ButtonSource down, ButtonSource left, ButtonSource right
  )
      : source(ButtonComposite2D{{up}, {down}, {left}, {right}}) { }

  BindingAxis2D(
    ButtonSources up, ButtonSources down, ButtonSources left, ButtonSources right
  )
      : source(ButtonComposite2D{
          std::move(up), std::move(down), std::move(left), std::move(right)
        }) { }

  explicit BindingAxis2D(ButtonComposite2D composite)
      : source(std::move(composite)) { }

  explicit BindingAxis2D(MouseVector vector) : source(vector) { }

  std::variant<ButtonComposite2D, MouseVector> source;
};

struct BindingAxis3D {
  BindingAxis3D() = default;

  BindingAxis3D(
    ButtonSource up,
    ButtonSource down,
    ButtonSource left,
    ButtonSource right,
    ButtonSource forward,
    ButtonSource backward
  )
      : source{{up}, {down}, {left}, {right}, {forward}, {backward}} { }

  BindingAxis3D(
    ButtonSources up,
    ButtonSources down,
    ButtonSources left,
    ButtonSources right,
    ButtonSources forward,
    ButtonSources backward
  )
      : source{
          std::move(up),
          std::move(down),
          std::move(left),
          std::move(right),
          std::move(forward),
          std::move(backward),
        } { }

  explicit BindingAxis3D(ButtonComposite3D composite)
      : source(std::move(composite)) { }

  ButtonComposite3D source;
};

using InputBinding =
  std::variant<BindingButton, BindingAxis, BindingAxis2D, BindingAxis3D>;

template <typename T>
concept InputBindingType =
  std::same_as<T, BindingButton> || std::same_as<T, BindingAxis>
  || std::same_as<T, BindingAxis2D> || std::same_as<T, BindingAxis3D>;

}
