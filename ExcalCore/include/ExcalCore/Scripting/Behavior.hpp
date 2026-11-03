#pragma once

#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRef.hpp"
#include "ExcalCore/Objects/GameObject.hpp"

namespace ExcalCore {

class Scene;

class Behavior {
public:
  Behavior(GameObject& obj, Scene& scene)
      : gameObject(obj),
        transform(obj.GetComponentRef<Transform>()),
        m_Scene(scene) { }

  virtual ~Behavior() = default;

  virtual void OnAwake() { }

  virtual void OnStart() { }
  virtual void OnUpdate() { }
  virtual void OnLateUpdate() { }
  virtual void OnFixedUpdate() { }

  virtual void OnEnable() { }
  virtual void OnDisable() { }

  virtual void OnDestroy() { }

  GameObject* CreateObject();

  GameObject& gameObject;
  ECSRef<Transform> transform;

private:
  Scene& m_Scene;
};

}
