#pragma once

#include <concepts>
#include <initializer_list>
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
  KeySource();
  KeySource(KeyCode key);

  KeyCode key = KeyCode::UNKNOWN;
};

struct MouseButtonSource {
  MouseButtonSource();
  MouseButtonSource(MouseButton button);

  MouseButton button = MouseButton::LEFT;
};

struct MouseScrollSource {
  MouseScrollSource();
  MouseScrollSource(ScrollDirection direction);

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
  BindingButton();

  template <typename... TSources>
    requires(
      sizeof...(TSources) > 0 &&
      (std::constructible_from<ButtonSource, TSources> && ...)
    )
  explicit BindingButton(TSources... sources);

  BindingButton(std::initializer_list<ButtonSource> sources);

  explicit BindingButton(ButtonSources sources);

  ButtonSources sources;
};

template <typename... TSources>
  requires(
    sizeof...(TSources) > 0 &&
    (std::constructible_from<ButtonSource, TSources> && ...)
  )
BindingButton::BindingButton(TSources... sources)
    : sources{ButtonSource(sources)...} { }

struct BindingAxis {
  BindingAxis();

  BindingAxis(ButtonSource negative, ButtonSource positive);

  BindingAxis(ButtonSources negative, ButtonSources positive);

  ButtonSources negative;
  ButtonSources positive;
};

struct BindingAxis2D {
  BindingAxis2D();

  BindingAxis2D(
    ButtonSource up, ButtonSource down, ButtonSource left, ButtonSource right
  );

  BindingAxis2D(
    ButtonSources up,
    ButtonSources down,
    ButtonSources left,
    ButtonSources right
  );

  explicit BindingAxis2D(ButtonComposite2D composite);

  explicit BindingAxis2D(MouseVector vector);

  std::variant<ButtonComposite2D, MouseVector> source;
};

struct BindingAxis3D {
  BindingAxis3D();

  BindingAxis3D(
    ButtonSource up,
    ButtonSource down,
    ButtonSource left,
    ButtonSource right,
    ButtonSource forward,
    ButtonSource backward
  );

  BindingAxis3D(
    ButtonSources up,
    ButtonSources down,
    ButtonSources left,
    ButtonSources right,
    ButtonSources forward,
    ButtonSources backward
  );

  explicit BindingAxis3D(ButtonComposite3D composite);

  ButtonComposite3D source;
};

using InputBinding =
  std::variant<BindingButton, BindingAxis, BindingAxis2D, BindingAxis3D>;

template <typename T>
concept InputBindingType =
  std::same_as<T, BindingButton> || std::same_as<T, BindingAxis> ||
  std::same_as<T, BindingAxis2D> || std::same_as<T, BindingAxis3D>;

}
