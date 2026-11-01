#pragma once

#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRef.hpp"
#include "ExcalCore/Objects/GameObject.hpp"

namespace ExcalCore {

class Behavior {
public:
  Behavior(GameObject& obj)
      : m_GameObject(obj), m_Transform(obj.GetComponentRef<Transform>()) { }

  virtual ~Behavior() = default;

  virtual void OnAwake() { }

  virtual void OnStart() { }
  virtual void OnUpdate() { }
  virtual void OnLateUpdate() { }
  virtual void OnFixedUpdate() { }

  virtual void OnEnable() { }
  virtual void OnDisable() { }

  virtual void OnDestroy() { }

  GameObject& m_GameObject;
  ECSRef<Transform> m_Transform;
};

}
