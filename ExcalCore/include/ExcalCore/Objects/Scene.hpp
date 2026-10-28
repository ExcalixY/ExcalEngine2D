#pragma once

#include <vector>

#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore {

class Scene {
public:
  explicit Scene() = default;
  virtual ~Scene() = default;

  virtual void OnUpdate(float dt) { }

  virtual void OnRender(Renderer2D& renderer) { }

private:
  std::vector<GameObject> m_GameObjects;
};

}
