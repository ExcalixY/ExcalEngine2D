#pragma once

#include "ExcalCore/ECS/ECSRegistry.hpp"

namespace ExcalCore::ECSSystems {

void UpdateLocalMatrices(ECSRegistry& ecs);
void UpdateGlobalMatrices(ECSRegistry& ecs);

}
