#pragma once

#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore {

// Copyable, non-movable handle to a Component in the ECS
template <ComponentType T>
class ECSRef final {
public:
  ECSRef(Entity e, ECSRegistry* ecs) : m_EntityHandle(e), m_ECS(ecs) { };
  ~ECSRef() = default;

  [[nodiscard]]
  T* Get() const {
    return m_ECS ? m_ECS->GetComponent<T>(m_EntityHandle) : nullptr;
  }

  T* operator->() {
    return Get();
  }
  T& operator*() {
    return *Get();
  }

  T* operator->() const {
    return Get();
  }
  T& operator*() const {
    return *Get();
  }

  explicit operator bool() const {
    return Get() != nullptr;
  }

private:
  Entity m_EntityHandle;
  ECSRegistry* m_ECS;
};

}
