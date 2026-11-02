#include "ExcalCore/ECS/Systems/HierarchyComponent.hpp"

#include "ExcalCore/ECS/Components/HierarchyComponent.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore::ECSSystems {

HierarchyComponent* DetachNodeFromOthers(ECSRegistry& ecs, Entity node) {
  auto hc_root = ecs.GetComponent<HierarchyComponent>(node);

  if (!hc_root) {
    Debug::LogWarning(
      Debug::CATEGORY_SCENE,
      "Provided Entity {} does not contain a HierarchyComponent.",
      node
    );
    return nullptr;
  }

  // Detach node
  auto hc_left = ecs.GetComponent<HierarchyComponent>(hc_root->prevSibling);
  auto hc_right = ecs.GetComponent<HierarchyComponent>(hc_root->nextSibling);
  auto hc_parent = ecs.GetComponent<HierarchyComponent>(hc_root->parent);

  if (hc_left) {
    hc_left->nextSibling = hc_root->nextSibling;
  }
  if (hc_right) {
    hc_right->prevSibling = hc_root->prevSibling;
  }
  if (hc_parent) {
    if (hc_parent->firstChild == node) {
      hc_parent->firstChild = hc_root->nextSibling;
    }
    if (hc_parent->lastChild == node) {
      hc_parent->lastChild = hc_root->prevSibling;
    }
  }

  hc_root->nextSibling = EMPTY_ENTITY;
  hc_root->prevSibling = EMPTY_ENTITY;

  return hc_root;
}

void SetParentOfEntity(ECSRegistry& ecs, Entity child, Entity parent) {
  auto hc_child = ecs.GetComponent<HierarchyComponent>(child);
  auto hc_parent = ecs.GetComponent<HierarchyComponent>(parent);

  if (!hc_child || !hc_parent) {
    if (!hc_child) {
      Debug::LogWarning(
        Debug::CATEGORY_SCENE,
        "Provided Child Entity {} does not contain a HierarchyComponent.",
        child
      );
    } else {
      Debug::LogWarning(
        Debug::CATEGORY_SCENE,
        "Provided Parent Entity {} does not contain a HierarchyComponent.",
        parent
      );
    }
    return;
  }

  // Recursive walk up parents
  for (Entity e_temp = parent; e_temp != EMPTY_ENTITY;
       e_temp = ecs.GetComponent<HierarchyComponent>(e_temp)->parent) {
    if (e_temp == child) {
      Debug::LogError(
        Debug::CATEGORY_SCENE,
        "Parent can not become a child inside of the same hierarchy as child.",
        child
      );

      return;
    }
  }

  hc_child = DetachNodeFromOthers(ecs, child);

  // Add Node to New Parent
  hc_child->parent = parent;

  if (hc_parent->lastChild != EMPTY_ENTITY) {
    auto hc_last_temp =
      ecs.GetComponent<HierarchyComponent>(hc_parent->lastChild);

    hc_last_temp->nextSibling = child;
    hc_child->prevSibling = hc_parent->lastChild;
  } else {
    hc_parent->firstChild = child;
  }

  hc_parent->lastChild = child;

  // Debug::Log("Moved child {} to {}", child, parent);
}

}
