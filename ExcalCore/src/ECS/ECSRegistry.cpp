#include "ExcalCore/ECS/ECSRegistry.hpp"

namespace ExcalCore {

Entity ECSRegistry::NewEntity() {
  Entity entity;

  if (!m_DeadIDs.empty()) {
    entity = {.id = m_DeadIDs.back(), .gen = ++m_IdGens[m_DeadIDs.back()]};

    m_DeadIDs.pop_back();
    m_IdGens[entity.id] = entity.gen;

    return entity;
  }

  entity = {.id = m_NextID++, .gen = 0};
  m_IdGens.push_back(0);
  return entity;
}

void ECSRegistry::DeleteEntity(Entity entity) {
  m_DeadIDs.push_back(entity.id);
}

}
