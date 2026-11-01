#include "ExcalCore/ECS/ECSRegistry.hpp"

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

  m_DeadIDs.push_back(entity.id);
  ++m_IdGens[entity.id];

  for (auto& [id, set] : m_DataSets) {
    set.get()->Remove(entity);
  }
}

bool ECSRegistry::IsAlive(const Entity entity) const {
  return entity.id < m_IdGens.size() && entity.gen == m_IdGens[entity.id];
}

}
