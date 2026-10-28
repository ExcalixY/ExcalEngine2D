#pragma once

#include <string_view>

#include "Event.hpp"

namespace ExcalCore {

struct WindowFocusEvent : Event {
  static constexpr std::string_view Name = "WindowFocus";

  bool focused = false;
};

}
