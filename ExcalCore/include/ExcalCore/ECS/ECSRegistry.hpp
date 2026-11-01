#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <memory>
#include <ranges>
#include <span>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Components/Component.hpp"
#include "Entity.hpp"
#include "ExcalCore/Utils/SparseSet.hpp"

namespace ExcalCore {

template <typename T>
concept ComponentType = std::derived_from<T, Component>;

class ECSRegistry final {
public:
  explicit ECSRegistry();
  ~ECSRegistry();

  [[nodiscard]]
  Entity NewEntity();
  void DeleteEntity(Entity entity);

  [[nodiscard]]
  bool IsAlive(const Entity entity) const;

  template <ComponentType... TComponents, typename TFunc>
    requires std::invocable<TFunc, const Entity&, TComponents*...>
  void Each(TFunc&& func);

  template <ComponentType... TComponents>
  [[nodiscard]]
  std::vector<Entity> Snapshot();

  template <ComponentType T, typename... TArgs>
  T* AddComponent(const Entity& entity, TArgs&&... args);

  template <ComponentType T>
  [[nodiscard]]
  T* GetComponent(const Entity& entity);

  template <ComponentType T>
  void RemoveComponent(const Entity& entity);

  template <ComponentType T>
  [[nodiscard]]
  bool HasComponent(const Entity& entity) const;

private:
  template <ComponentType T>
  [[nodiscard]]
  SparseSet<T>& GetSet();

  std::vector<uint32_t> m_DeadIDs;
  std::vector<uint32_t> m_IdGens;

  uint32_t m_NextID = 0;

  std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> m_DataSets;
};

template <ComponentType... TComponents, typename TFunc>
  requires std::invocable<TFunc, const Entity&, TComponents*...>
void ECSRegistry::Each(TFunc&& func) {
  std::tuple<SparseSet<TComponents>&...> pools = {GetSet<TComponents>()...};

  std::span<const Entity> entities = std::ranges::min(
    {std::get<SparseSet<TComponents>&>(pools).GetEntities()...},
    {},
    std::ranges::size
  );

  for (Entity e : entities) {
    std::tuple<TComponents*...> components;

    bool has_all =
      ((std::get<TComponents*>(components) =
          (std::get<SparseSet<TComponents>&>(pools).TryGet(e))) &&
       ...);

    if (has_all) {
      std::apply([&](TComponents*... c) { func(e, c...); }, components);
    }
  }
}

template <ComponentType... TComponents>
std::vector<Entity> ECSRegistry::Snapshot() {
  std::vector<Entity> entity_list;

  std::tuple<SparseSet<TComponents>&...> pools = {GetSet<TComponents>()...};

  std::span<const Entity> entities = std::ranges::min(
    {std::get<SparseSet<TComponents>&>(pools).GetEntities()...},
    {},
    std::ranges::size
  );

  for (Entity e : entities) {
    std::tuple<TComponents*...> components;

    bool has_all =
      ((std::get<TComponents*>(components) =
          (std::get<SparseSet<TComponents>&>(pools).TryGet(e))) &&
       ...);

    if (has_all) {
      entity_list.push_back(e);
    }
  }

  return entity_list;
}

template <ComponentType T, typename... TArgs>
T* ECSRegistry::AddComponent(const Entity& entity, TArgs&&... args) {
  if (!IsAlive(entity)) {
    return nullptr;
  }

  auto& set = GetSet<T>();
  return set.Emplace(entity, std::forward<TArgs>(args)...);
}

template <ComponentType T>
T* ECSRegistry::GetComponent(const Entity& entity) {
  auto& set = GetSet<T>();
  return set.TryGet(entity);
}

template <ComponentType T>
void ECSRegistry::RemoveComponent(const Entity& entity) {
  auto it = m_DataSets.find(std::type_index(typeid(T)));
  if (it == m_DataSets.end()) {
    return;
  }

  auto& set = *static_cast<SparseSet<T>*>(it->second.get());
  set.Remove(entity);
}

template <ComponentType T>
[[nodiscard]]
bool ECSRegistry::HasComponent(const Entity& entity) const {
  auto it = m_DataSets.find(std::type_index(typeid(T)));
  if (it == m_DataSets.end()) {
    return false;
  }

  return static_cast<const SparseSet<T>&>(*it->second).Contains(entity);
}

template <ComponentType T>
SparseSet<T>& ECSRegistry::GetSet() {
  auto [it, inserted] =
    m_DataSets.try_emplace(std::type_index(typeid(T)), nullptr);

  if (inserted) {
    it->second = std::make_unique<SparseSet<T>>();
  }

  return static_cast<SparseSet<T>&>(*it->second);
}

}
