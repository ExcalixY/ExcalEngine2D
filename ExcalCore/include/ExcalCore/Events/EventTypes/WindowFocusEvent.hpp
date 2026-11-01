#pragma once

#include <string_view>

#include "ExcalCore/Events/Event.hpp"

namespace ExcalCore {

struct WindowFocusEvent : Event {
  static constexpr std::string_view Name = "WindowFocus";

  bool focused = false;
};

}
