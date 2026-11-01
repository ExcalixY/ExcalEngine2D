#include "ExcalCore/ECS/Systems/Transform.hpp"

#include <cmath>
#include <glm/ext/vector_float3.hpp>

#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore::ECSSystems {

void UpdateLocalMatrices(ECSRegistry& ecs) {
  ecs.Each<Transform>([](Entity, Transform* t) {
    float c = std::cos(t->rot);
    float s = std::sin(t->rot);

    t->localMatrix = {
      glm::vec3(t->scale.x * c, t->scale.x * s, 0.f),   // column 0: x-axis
      glm::vec3(-t->scale.y * s, t->scale.y * c, 0.f),  // column 1: y-axis
      glm::vec3(t->pos.x, t->pos.y, 1.f)                // column 2: position
    };
  });
}

void UpdateGlobalMatrices(ECSRegistry& ecs) {
  ecs.Each<Transform, WorldTransform>(
    [](Entity, const Transform* t, WorldTransform* wt) {
      wt->worldMatrix = t->localMatrix;
    }
  );
}

}
