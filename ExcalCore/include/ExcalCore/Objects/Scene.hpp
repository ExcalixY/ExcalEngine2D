#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ExcalCore/ECS/Components/BehaviorComponent.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Scripting/Behavior.hpp"

namespace ExcalCore {

class Scene {
public:
  explicit Scene();

  Scene(const Scene&) = default;
  Scene(Scene&&) = delete;
  Scene& operator=(const Scene&) = default;
  Scene& operator=(Scene&&) = delete;

  virtual ~Scene();

  virtual void OnInit() = 0;

  void OnFrameStart();
  void OnUpdate(float dt);
  void OnRender(Renderer2D& renderer);
  void OnFrameEnd();

  void UnloadAllObjects();

  [[nodiscard]]
  GameObject* CreateObject(
    const std::string& name, GameObject* parent = nullptr
  );

  template <typename T, typename... TArgs>
    requires std::derived_from<T, Behavior>
  T* AttachBehavior(GameObject* obj, TArgs&&... args) {
    if (!obj) {
      return nullptr;
    }

    auto comp = m_ECS.AddComponent<BehaviorComponent>(obj->m_EntityHandle);
    if (!comp || comp->behavior) {
      return nullptr;
    }

    comp->behavior =
      std::make_unique<T>(*obj, m_ECS, std::forward<TArgs>(args)...);

    comp->behavior->OnAwake();
    obj->EnableObjectFlags(GameObject::Flags::CONTAINS_BEHAVIOR);

    return static_cast<T*>(comp->behavior.get());
  }

protected:
  std::vector<std::unique_ptr<GameObject>> m_GameObjects;
  size_t m_ObjCount = 0;

private:
  ECSRegistry m_ECS;
};

}
