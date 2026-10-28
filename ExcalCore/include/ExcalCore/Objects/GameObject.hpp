#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Components/Component.hpp"

namespace ExcalCore {

class GameObject {
public:
  virtual ~GameObject() = default;

  virtual void OnStart() { }

  virtual void OnUpdate(float dt) { }

  virtual void OnEnabled() { }
  virtual void OnDisabled() { }

  const std::string& GetName() const {
    return m_Name;
  }

  template <typename T, typename... TArgs>
    requires std::derived_from<T, ComponentBase>
  T* AddComponent(TArgs&&... args);

  template <typename T>
  T* GetComponent() const;

  template <typename T>
  bool HasComponent() const;

protected:
  friend class Scene;

  std::string m_Name;
  std::vector<std::unique_ptr<ComponentBase>> m_Components;
};

template <typename T, typename... TArgs>
  requires std::derived_from<T, ComponentBase>
T* GameObject::AddComponent(TArgs&&... args) {
  auto component = std::make_unique<T>(std::forward<TArgs>(args)...);
  T* raw = component.get();

  raw->m_Owner = this;
  m_Components.push_back(std::move(component));

  raw->OnAttach();
  return raw;
}

template <typename T>
T* GameObject::GetComponent() const {
  for (const auto& component : m_Components) {
    if (auto* found = dynamic_cast<T*>(component.get())) {
      return found;
    }
  }
  return nullptr;
}

template <typename T>
bool GameObject::HasComponent() const {
  return GetComponent<T>() != nullptr;
}

}
