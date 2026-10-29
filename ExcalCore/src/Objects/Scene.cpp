#include "ExcalCore/Objects/Scene.hpp"

namespace ExcalCore {

Scene::Scene(ECSRegistry& ecs) : m_ECS(ecs) { }

void Scene::OnUpdate(float dt) {
  for (auto& obj : m_GameObjects) {
    obj.OnUpdate(dt);
  }
}

GameObject* Scene::CreateObject(const std::string& name) {
  return nullptr;
}

}
