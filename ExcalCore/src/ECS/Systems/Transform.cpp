#include "ExcalCore/ECS/Systems/Transform.hpp"

#include <cmath>
#include <glm/ext/vector_float3.hpp>

#include "ExcalCore/ECS/Components/HierarchyComponent.hpp"
#include "ExcalCore/ECS/Components/ObjectEnabledComponent.hpp"
#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"
#include "ExcalCore/ECS/Systems/HierarchyComponent.hpp"

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

void UpdateGlobalMatrices(ECSRegistry& ecs, Entity root) {
  ECSSystems::WalkHierarchy<TreeWalkOrder::PRE>(
    ecs, root, [&ecs, root](HierarchyComponent* hc_cur, Entity e_cur) -> bool {
      if (e_cur == root) {
        return true;
      }

      if (!ecs.HasComponent<ObjectEnabledComponent>(e_cur)) {
        return false;
      }

      auto t = ecs.GetComponent<Transform>(e_cur);
      auto tw = ecs.GetComponent<WorldTransform>(e_cur);

      if (root == hc_cur->parent) {
        tw->worldMatrix = t->localMatrix;
      } else {
        auto tw_parent = ecs.GetComponent<WorldTransform>(hc_cur->parent);
        tw->worldMatrix = tw_parent->worldMatrix * t->localMatrix;
      }

      return true;
    }
  );
}

}
