#pragma once

#include <string>

#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore {

class GameObject final {
public:
  GameObject(const std::string& name, Entity entity)
      : m_Name(name), m_QueueDestroy(false), m_EntityHandle(entity) { }

  ~GameObject() = default;

  const std::string& GetName() const;

  template <ComponentType T, typename... TArgs>
  T* AddComponent(TArgs&&... args);

  template <ComponentType T>
  T* GetComponent() const;

  template <ComponentType T>
  bool HasComponent() const;

protected:
  friend class Scene;

  std::string m_Name;
  Entity m_EntityHandle;
  bool m_QueueDestroy;
};

template <ComponentType T, typename... TArgs>
T* GameObject::AddComponent(TArgs&&... args) {
  return nullptr;
}

template <ComponentType T>
T* GameObject::GetComponent() const {
  return nullptr;
}

template <ComponentType T>
bool GameObject::HasComponent() const {
  return GetComponent<T>() != nullptr;
}

}
