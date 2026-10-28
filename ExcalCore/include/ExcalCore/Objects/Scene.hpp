#pragma once

#include <vector>

#include "Components/Component.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore {

class Scene {
public:
  explicit Scene() = default;
  virtual ~Scene() = default;

  virtual void OnUpdate(float dt) {
    for (auto& updateable : m_UpdateableComponents) {
      updateable->OnUpdate(dt);
    }

    for (auto& obj : m_GameObjects) {
      obj->OnUpdate(dt);
    }
  }

  virtual void OnRender(Renderer2D& renderer) {
    for (auto& renderable : m_RenderableComponents) {
      renderable->OnRender(renderer);
    }
  }

  GameObject* CreateObject(const std::string& name) { }

protected:
  std::vector<std::unique_ptr<GameObject>> m_GameObjects;

  std::vector<IRenderable*> m_RenderableComponents;
  std::vector<IUpdatable*> m_UpdateableComponents;
  std::vector<IStartable*> m_StartableComponents;
};

}
