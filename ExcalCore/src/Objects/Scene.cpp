#include "ExcalCore/Objects/Scene.hpp"

namespace ExcalCore {

Scene::Scene(ECSRegistry& ecs) : m_ECS(ecs) { }

void Scene::OnUpdate(float dt) { }

GameObject* Scene::CreateObject(const std::string& name) {
  return &m_GameObjects.emplace_back(name, m_ECS.NewEntity());
}

void Scene::OnFrameEnd() {
  for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); ++it) {
    if (it->m_QueueDestroy) {
      it = m_GameObjects.erase(it);
    }
  }
}

}
