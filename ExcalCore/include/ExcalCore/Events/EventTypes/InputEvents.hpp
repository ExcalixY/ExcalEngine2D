#pragma once

#include "ExcalCore/Events/Event.hpp"

namespace ExcalCore {

struct InputEvent : Event { };

struct KeyboardEvent : InputEvent { };

struct MouseEvent : InputEvent { };

}
