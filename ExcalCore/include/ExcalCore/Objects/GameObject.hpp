#pragma once

#include <string>

#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore {

class GameObject final {
public:
  virtual ~GameObject() = default;

  virtual void OnStart() { }

  virtual void OnUpdate(float dt) { }

  virtual void OnEnabled() { }
  virtual void OnDisabled() { }

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
