#pragma once

#include <string_view>

#include "ExcalCore/ECS/Components/Component.hpp"
#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore {

struct HierarchyComponent : Component {
  static constexpr std::string_view Name = "HierarchyComponent";

  Entity parent = EMPTY_ENTITY;
  Entity nextSibling = EMPTY_ENTITY;
  Entity prevSibling = EMPTY_ENTITY;
  Entity firstChild = EMPTY_ENTITY;
  Entity lastChild = EMPTY_ENTITY;
};

}
