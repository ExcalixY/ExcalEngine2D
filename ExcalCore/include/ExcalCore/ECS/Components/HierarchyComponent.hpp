#pragma once

#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore {

struct [[deprecated(
  "Hierarchies are not yet implemented. Will do absolutely nothing."
)]] HierarchyComponent {
  Entity current;
  Entity parent;
  Entity next;
  Entity child;
};

}
