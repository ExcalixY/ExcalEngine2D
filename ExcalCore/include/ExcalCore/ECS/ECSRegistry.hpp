#pragma once

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

  Entity NewEntity();
  void DeleteEntity(Entity entity);

  bool IsAlive(const Entity entity) const;

  template <ComponentType... TComponents, typename TFunc>
    requires std::invocable<TFunc, const Entity&, TComponents*...>
  void Each(TFunc&& func);

  template <ComponentType T, typename... TArgs>
  T* AddComponent(const Entity& entity, TArgs&&... args);

  template <ComponentType T>
  T* GetComponent(const Entity& entity);

  template <ComponentType T>
  bool HasComponent(const Entity& entity) const;

  template <ComponentType T>
  bool HasComponent(const Entity& entity);

private:
  template <ComponentType T>
  SparseSet<T>& GetSet();

  std::vector<uint32_t> m_DeadIDs;
  std::vector<uint32_t> m_IdGens;

  uint32_t m_NextID;

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

template <ComponentType T, typename... TArgs>
T* ECSRegistry::AddComponent(const Entity& entity, TArgs&&... args) {
  auto& set = GetSet<T>();
  return set.Emplace(entity, std::forward<TArgs>(args)...);
}

template <ComponentType T>
T* ECSRegistry::GetComponent(const Entity& entity) {
  auto& set = GetSet<T>();
  return set.TryGet(entity);
}

template <ComponentType T>
bool ECSRegistry::HasComponent(const Entity& entity) const {
  return GetComponent<T>(entity) != nullptr;
}

template <ComponentType T>
bool ECSRegistry::HasComponent(const Entity& entity) {
  return GetComponent<T>(entity) != nullptr;
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
