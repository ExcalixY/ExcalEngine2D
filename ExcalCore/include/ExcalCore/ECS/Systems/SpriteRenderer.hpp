#pragma once

#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore::ECSSystems {

void IssueDrawCommands(ECSRegistry& ecs, Renderer2D& renderer);

}
