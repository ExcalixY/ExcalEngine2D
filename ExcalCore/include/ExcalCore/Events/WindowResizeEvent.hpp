#pragma once

#include <string_view>

#include "Event.hpp"

namespace ExcalCore {

struct WindowResizeEvent : Event {
  static constexpr std::string_view Name = "WindowResize";

  int width = 0;
  int height = 0;
};

}
