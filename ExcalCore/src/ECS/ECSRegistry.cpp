#include "ExcalCore/ECS/ECSRegistry.hpp"

#include <algorithm>
#include <typeindex>
#include <utility>

#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore {

ECSRegistry::ECSRegistry() = default;
ECSRegistry::~ECSRegistry() = default;

Entity ECSRegistry::NewEntity() {
  Entity entity = {};

  if (!m_DeadIDs.empty()) {
    entity = {.id = m_DeadIDs.back(), .gen = m_IdGens[m_DeadIDs.back()]};

    m_DeadIDs.pop_back();
    m_IdGens[entity.id] = entity.gen;

    return entity;
  }

  entity = {.id = m_NextID++, .gen = 0};
  m_IdGens.push_back(0);
  return entity;
}

void ECSRegistry::DeleteEntity(Entity entity) {
  if (!IsAlive(entity)) {
    return;
  }

  ++m_IdGens[entity.id];

  m_RemovalQueue.emplace_back(entity, typeid(void));
}

bool ECSRegistry::IsAlive(const Entity entity) const {
  return entity.id < m_IdGens.size() && entity.gen == m_IdGens[entity.id];
}

void ECSRegistry::FlushRemoval() {
  std::for_each(
    m_RemovalQueue.begin(),
    m_RemovalQueue.end(),
    [this](std::pair<Entity, std::type_index>& entry) {
      if (entry.second == typeid(void)) {
        m_DeadIDs.push_back(entry.first.id);

        for (auto& [id, set] : m_DataSets) {
          set.get()->Remove(entry.first);
        }
      } else {
        auto it = m_DataSets.find(entry.second);
        if (it != m_DataSets.end()) {
          it->second.get()->Remove(entry.first);
        }
      }
    }
  );

  m_RemovalQueue.clear();
}

}
