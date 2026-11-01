#pragma once

#include <memory>

#include "ExcalCore/ECS/Components/Component.hpp"

namespace ExcalCore {

class Behavior;

struct BehaviorComponent : Component {
  std::unique_ptr<Behavior> behavior;
  bool started = false;
};

}
