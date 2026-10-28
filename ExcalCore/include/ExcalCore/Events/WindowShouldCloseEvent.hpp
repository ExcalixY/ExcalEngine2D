#pragma once

#include <string_view>

#include "Event.hpp"

namespace ExcalCore {

struct WindowShouldCloseEvent : Event {
  static constexpr std::string_view Name = "WindowShouldClose";
};

}
