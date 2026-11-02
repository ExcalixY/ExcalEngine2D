#pragma once

#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore::ECSSystems {

void UpdateLocalMatrices(ECSRegistry& ecs);
void UpdateGlobalMatrices(ECSRegistry& ecs, Entity root);

}
