#include "ExcalCore/Input/InputBindings.hpp"

namespace ExcalCore {

KeySource::KeySource(KeyCode key) : key(key) { }

MouseButtonSource::MouseButtonSource(MouseButton button) : button(button) { }

MouseScrollSource::MouseScrollSource(ScrollDirection direction)
    : direction(direction) { }

BindingButton::BindingButton(std::initializer_list<ButtonSource> sources)
    : sources(sources) { }

BindingButton::BindingButton(ButtonSources sources)
    : sources(std::move(sources)) { }

BindingAxis::BindingAxis(ButtonSource negative, ButtonSource positive)
    : negative{negative}, positive{positive} { }

BindingAxis::BindingAxis(ButtonSources negative, ButtonSources positive)
    : negative(std::move(negative)), positive(std::move(positive)) { }

BindingAxis2D::BindingAxis2D(
  ButtonSource up, ButtonSource down, ButtonSource left, ButtonSource right
)
    : source(ButtonComposite2D{{up}, {down}, {left}, {right}}) { }

BindingAxis2D::BindingAxis2D(
  ButtonSources up, ButtonSources down, ButtonSources left, ButtonSources right
)
    : source(ButtonComposite2D{
        std::move(up), std::move(down), std::move(left), std::move(right)
      }) { }

BindingAxis2D::BindingAxis2D(ButtonComposite2D composite)
    : source(std::move(composite)) { }

BindingAxis2D::BindingAxis2D(MouseVector vector) : source(vector) { }

BindingAxis3D::BindingAxis3D(
  ButtonSource up,
  ButtonSource down,
  ButtonSource left,
  ButtonSource right,
  ButtonSource forward,
  ButtonSource backward
)
    : source{{up}, {down}, {left}, {right}, {forward}, {backward}} { }

BindingAxis3D::BindingAxis3D(
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

BindingAxis3D::BindingAxis3D(ButtonComposite3D composite)
    : source(std::move(composite)) { }

KeySource::KeySource() = default;

MouseButtonSource::MouseButtonSource() = default;

MouseScrollSource::MouseScrollSource() = default;

BindingButton::BindingButton() = default;

BindingAxis::BindingAxis() = default;

BindingAxis2D::BindingAxis2D() = default;

BindingAxis3D::BindingAxis3D() = default;

}
