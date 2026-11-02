#pragma once

#include <concepts>
#include <type_traits>

#include "ExcalCore/ECS/Components/HierarchyComponent.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore::ECSSystems {

HierarchyComponent* DetachNodeFromOthers(ECSRegistry& ecs, Entity node);
void SetParentOfEntity(ECSRegistry& ecs, Entity child, Entity parent);
void DestroyHierarchyNode(ECSRegistry& ecs, Entity root);

enum class TreeWalkOrder { PRE, POST };

template <TreeWalkOrder Order, typename TFunc>
  requires std::invocable<TFunc&, HierarchyComponent*, Entity>
void WalkHierarchy(ECSRegistry& ecs, Entity root, TFunc&& func) {
  if (!ecs.GetComponent<HierarchyComponent>(root)) {
    return;
  }

  auto hierarchy = [&ecs](Entity e) -> HierarchyComponent& {
    return *ecs.GetComponent<HierarchyComponent>(e);
  };

  if constexpr (Order == TreeWalkOrder::PRE) {
    constexpr bool can_skip = std::
      same_as<std::invoke_result_t<TFunc&, HierarchyComponent*, Entity>, bool>;

    Entity node = root;

    while (node != EMPTY_ENTITY) {
      bool descend = true;

      if constexpr (can_skip) {
        descend = func(&hierarchy(node), node);
      } else {
        func(&hierarchy(node), node);
      }

      if (descend && hierarchy(node).firstChild != EMPTY_ENTITY) {
        node = hierarchy(node).firstChild;
        continue;
      }

      while (node != root && hierarchy(node).nextSibling == EMPTY_ENTITY) {
        node = hierarchy(node).parent;
      }

      node = (node == root) ? EMPTY_ENTITY : hierarchy(node).nextSibling;
    }
  } else if constexpr (Order == TreeWalkOrder::POST) {
    auto leftmost = [&](Entity n) {
      while (hierarchy(n).firstChild != EMPTY_ENTITY) {
        n = hierarchy(n).firstChild;
      }
      return n;
    };

    Entity node = leftmost(root);

    while (true) {
      Entity next = hierarchy(node).nextSibling;
      Entity parent = hierarchy(node).parent;
      bool done = (node == root);

      func(&hierarchy(node), node);

      if (done) {
        break;
      }

      node = (next != EMPTY_ENTITY) ? leftmost(next) : parent;
    }
  }
}

}
