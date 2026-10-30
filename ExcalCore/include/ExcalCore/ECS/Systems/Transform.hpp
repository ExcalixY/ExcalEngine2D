#pragma once

#include <cmath>

#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore::ECSSystems {

void UpdateLocalMatrices(ECSRegistry& ecs);
void UpdateGlobalMatrices(ECSRegistry& ecs);

}
