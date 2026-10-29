#pragma once

#include <vector>

#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore {

class Scene {
public:
  explicit Scene(ECSRegistry& ecs);
  virtual ~Scene() = default;

  virtual void OnUpdate(float dt);

  virtual void OnRender(Renderer2D& renderer) { }

  GameObject* CreateObject(const std::string& name);

protected:
  std::vector<GameObject> m_GameObjects;
  ECSRegistry& m_ECS;
};

}
