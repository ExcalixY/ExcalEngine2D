#pragma once

#include <concepts>
#include <cstdint>
#include <memory>
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

  Entity NewEntity();
  void DeleteEntity(Entity entity);

  template <ComponentType... TComponents, typename TFunc>
  void Each(TFunc&& func);

  template <ComponentType T, typename... TArgs>
  T* AddComponent(const Entity& entity);

  template <ComponentType T>
  T* GetComponent(const Entity& entity);

  template <ComponentType T>
  bool HasComponent(const Entity& entity) const;

private:
  template <ComponentType T>
  SparseSet<T>& GetSet();

  std::vector<uint32_t> m_DeadIDs;
  std::vector<uint32_t> m_IdGens;

  uint32_t m_NextID;

  std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> m_DataSets;
};

template <ComponentType... TComponents, typename TFunc>
void ECSRegistry::Each(TFunc&& func) { }

template <ComponentType T, typename... TArgs>
T* ECSRegistry::AddComponent(const Entity& entity) { }

template <ComponentType T>
T* ECSRegistry::GetComponent(const Entity& entity) { }

template <ComponentType T>
bool ECSRegistry::HasComponent(const Entity& entity) const {
  return GetComponent<T>(entity) != nullptr;
}

template <ComponentType T>
SparseSet<T>& ECSRegistry::GetSet() { }

}
