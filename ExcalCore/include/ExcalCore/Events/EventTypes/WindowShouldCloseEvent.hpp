#pragma once

#include <string_view>

#include "ExcalCore/Events/Event.hpp"

namespace ExcalCore {

struct WindowShouldCloseEvent : Event {
  static constexpr std::string_view Name = "WindowShouldClose";
};

}
