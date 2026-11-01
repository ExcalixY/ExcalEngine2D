#pragma once

#include <bitset>
#include <concepts>
#include <cstdint>
#include <string>
#include <utility>

#include "ExcalCore/ECS/Components/BehaviorComponent.hpp"
#include "ExcalCore/ECS/Components/HierarchyComponent.hpp"
#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRef.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore {

class GameObject final {
public:
  enum class Flags : uint8_t {
    QUEUE_DESTROY,
    QUEUE_ENABLE,
    QUEUE_DISABLE,
    ENABLED,

    CONTAINS_BEHAVIOR
  };

  GameObject(Entity entity, const std::string& name, ECSRegistry& ecs)
      : m_EntityHandle(entity), m_ECSRegistry(ecs), m_Name(name) { }

  ~GameObject() = default;

  [[nodiscard]]
  const std::string& GetName() const {
    return m_Name;
  }

  void Delete() {
    EnableObjectFlags(Flags::QUEUE_DESTROY);
  }

  void SetEnabled(bool enabled) {
    EnableObjectFlags((enabled ? Flags::QUEUE_ENABLE : Flags::QUEUE_DISABLE));
  }

  template <ComponentType T>
  ECSRef<T> GetComponentRef() const;

  template <ComponentType T, typename... TArgs>
    requires(!std::same_as<T, BehaviorComponent>)
  T* AddComponent(TArgs&&... args);

  template <ComponentType T>
  [[nodiscard]]
  T* GetComponent() const;

  template <ComponentType T>
  [[nodiscard]]
  bool HasComponent() const;

  template <ComponentType T>
    requires(
      !std::same_as<T, Transform> && !std::same_as<T, WorldTransform>
      // && !std::same_as<T, HierarchyComponent>
      //  TODO: Remove comment after implementing HierarchyComponent
    )
  void RemoveComponent();

  [[nodiscard]]
  Entity GetHandle() {
    return m_EntityHandle;
  }

private:
  friend class Scene;

  template <typename... TFlags>
    requires(std::same_as<TFlags, Flags> && ...)
  [[nodiscard]]
  bool HasObjectFlags(TFlags... flags) const {
    return ((m_Flags.test(std::to_underlying(flags)) && ...));
  }

  template <typename... TFlags>
    requires(std::same_as<TFlags, Flags> && ...)
  void EnableObjectFlags(TFlags... flags) {
    (m_Flags.set(std::to_underlying(flags), true), ...);
  }

  template <typename... TFlags>
    requires(std::same_as<TFlags, Flags> && ...)
  void DisableObjectFlags(TFlags... flags) {
    (m_Flags.set(std::to_underlying(flags), false), ...);
  }

  std::bitset<128> m_Tags;
  Entity m_EntityHandle;
  ECSRegistry& m_ECSRegistry;

  std::bitset<8> m_Flags;

  std::string m_Name;
};

template <ComponentType T, typename... TArgs>
  requires(!std::same_as<T, BehaviorComponent>)
T* GameObject::AddComponent(TArgs&&... args) {
  return m_ECSRegistry.AddComponent<T>(
    m_EntityHandle, std::forward<TArgs>(args)...
  );
}

template <ComponentType T>
[[nodiscard]]
T* GameObject::GetComponent() const {
  return m_ECSRegistry.GetComponent<T>(m_EntityHandle);
}

template <ComponentType T>
[[nodiscard]]
bool GameObject::HasComponent() const {
  return m_ECSRegistry.HasComponent<T>(m_EntityHandle);
}

template <ComponentType T>
  requires(
    !std::same_as<T, Transform> && !std::same_as<T, WorldTransform>
    // && !std::same_as<T, HierarchyComponent>
    //  TODO: Remove comment after implementing HierarchyComponent
  )
void GameObject::RemoveComponent() {
  m_ECSRegistry.RemoveComponent<T>(m_EntityHandle);

  if constexpr (std::same_as<T, BehaviorComponent>) {
    DisableObjectFlags(GameObject::Flags::CONTAINS_BEHAVIOR);
  }
}

template <ComponentType T>
ECSRef<T> GameObject::GetComponentRef() const {
  return ECSRef<T>(m_EntityHandle, &m_ECSRegistry);
}

}
