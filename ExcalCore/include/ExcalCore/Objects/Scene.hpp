#pragma once

#include <vector>

#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore {

class Scene {
public:
  explicit Scene(ECSRegistry& ecs);
  virtual ~Scene() = default;

  virtual void OnInit() = 0;
  virtual void OnFrameStart() { };
  virtual void OnUpdate(float dt) = 0;
  virtual void OnRender(Renderer2D& renderer) = 0;
  virtual void OnFrameEnd();

  GameObject* CreateObject(const std::string& name);

protected:
  std::vector<GameObject> m_GameObjects;
  ECSRegistry& m_ECS;
};

}
