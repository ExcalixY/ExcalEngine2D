#pragma once

#include "ExcalCore/ECS/Components/Component.hpp"
#include "ExcalCore/Objects/GameObject.hpp"

namespace ExcalCore {

struct GameObjectRef : Component {
  GameObject* object = nullptr;
};

};
