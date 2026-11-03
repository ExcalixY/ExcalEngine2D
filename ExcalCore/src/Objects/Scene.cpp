#include "ExcalCore/Objects/Scene.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>

#include "ExcalCore/ECS/Components/BehaviorComponent.hpp"
#include "ExcalCore/ECS/Components/GameObjectRef.hpp"
#include "ExcalCore/ECS/Components/HierarchyComponent.hpp"
#include "ExcalCore/ECS/Components/ObjectEnabledComponent.hpp"
#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"
#include "ExcalCore/ECS/Systems/HierarchyComponent.hpp"
#include "ExcalCore/ECS/Systems/SpriteRenderer.hpp"
#include "ExcalCore/ECS/Systems/Transform.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Systems/TimeSystem.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

Scene::Scene() : m_ECS(), m_RootEntity(m_ECS.NewEntity()) {
  m_ECS.AddComponent<HierarchyComponent>(m_RootEntity);
};

Scene::~Scene() {
  UnloadAllObjects();
}

GameObject* Scene::CreateObject(const std::string& name, GameObject* parent) {
  Entity e = m_ECS.NewEntity();

  m_ECS.AddComponent<Transform>(e);
  m_ECS.AddComponent<WorldTransform>(e);

  m_ECS.AddComponent<HierarchyComponent>(e);
  ECSSystems::SetParentOfEntity(m_ECS, e, parent ? parent->m_EntityHandle : m_RootEntity);

  auto ptr = std::make_unique<GameObject>(e, name, m_ECS);

  ptr->SetEnabled(true);

  m_ECS.AddComponent<GameObjectRef>(e)->object = ptr.get();

  m_GameObjects.push_back(std::move(ptr));

  return m_GameObjects.back().get();
}

void Scene::DestroyObject(GameObject* obj) {
  if (!obj) {
    return;
  }

  obj->Destroy();
}

void Scene::EnableHierarchy(Entity root, GameObject* obj) {
  using enum GameObject::Flags;

  obj->DisableObjectFlags(QUEUE_ENABLE);
  obj->EnableObjectFlags(ENABLED, ENABLED_IN_HIERARCHY);

  m_ECS.AddComponent<ObjectEnabledComponent>(root);
  auto bc = obj->GetComponent<BehaviorComponent>();
  if (bc) {
    bc->behavior->OnEnable();
  }

  ECSSystems::WalkHierarchy<ECSSystems::TreeWalkOrder::POST>(
    m_ECS, root, [this, root](HierarchyComponent* hc, Entity e) {
      if (e == root) {
        return true;
      }

      auto ref = m_ECS.GetComponent<GameObjectRef>(e);
      auto obj = ref->object;

      if (obj->HasObjectFlags(NEW_OBJECT)) {
        return false;
      }

      if (obj->HasObjectFlags(QUEUE_DISABLE)) {
        obj->DisableObjectFlags(ENABLED, ENABLED_IN_HIERARCHY, QUEUE_DISABLE);
        return false;
      }

      if (!obj->HasObjectFlags(ENABLED)) {
        return false;
      }

      auto bc = obj->GetComponent<BehaviorComponent>();

      if (bc) {
        bc->behavior->OnEnable();
      }
      m_ECS.AddComponent<ObjectEnabledComponent>(e);

      return true;
    }
  );
}

void Scene::DisableHierarachy(Entity root, GameObject* obj) {
  using enum GameObject::Flags;

  obj->DisableObjectFlags(ENABLED, ENABLED_IN_HIERARCHY, QUEUE_DISABLE);
  m_ECS.RemoveComponent<ObjectEnabledComponent>(root);
  auto bc = obj->GetComponent<BehaviorComponent>();
  if (bc) {
    bc->behavior->OnDisable();
  }

  ECSSystems::WalkHierarchy<ECSSystems::TreeWalkOrder::POST>(
    m_ECS, root, [this, root](HierarchyComponent* hc, Entity e) {
      if (e == root) {
        return true;
      }

      auto ref = m_ECS.GetComponent<GameObjectRef>(e);
      auto obj = ref->object;

      if (obj->HasObjectFlags(NEW_OBJECT)) {
        return false;
      }

      auto bc = obj->GetComponent<BehaviorComponent>();

      if (obj->HasObjectFlags(QUEUE_DISABLE)) {
        if (obj->HasObjectFlags(ENABLED)) {
          m_ECS.RemoveComponent<ObjectEnabledComponent>(e);
          bc->behavior->OnDisable();
        }

        DisableHierarachy(e, obj);
        return false;
      }

      if (!obj->HasObjectFlags(ENABLED)) {
        obj->DisableObjectFlags(ENABLED_IN_HIERARCHY);
        return false;
      }

      if (bc) {
        bc->behavior->OnDisable();
      }
      m_ECS.RemoveComponent<ObjectEnabledComponent>(e);

      return true;
    }
  );
}

void Scene::OnFrameStart() {
  using enum GameObject::Flags;

  m_ObjCount = m_GameObjects.size();

  ECSSystems::WalkHierarchy<ECSSystems::TreeWalkOrder::PRE>(
    m_ECS, m_RootEntity, [this](HierarchyComponent* hc, Entity e) -> bool {
      if (e == m_RootEntity) {
        return true;
      }

      auto ref = m_ECS.GetComponent<GameObjectRef>(e);
      auto obj = ref->object;

      if (obj->HasObjectFlags(QUEUE_ENABLE, QUEUE_DISABLE)) {
        obj->DisableObjectFlags(QUEUE_ENABLE, QUEUE_DISABLE);
      }

      if (obj->HasObjectFlags(NEW_OBJECT)) {
        return false;
      } else if (obj->HasObjectFlags(QUEUE_DISABLE)) {
        DisableHierarachy(e, obj);
        return false;
      } else if (obj->HasObjectFlags(QUEUE_ENABLE)) {
        EnableHierarchy(e, obj);
        return true;
      }

      return true;
    }
  );
}

void Scene::OnUpdate(float dt) {
  for (size_t i = 0; i < m_ObjCount; ++i) {
    for (int i = 0; i < Time::GetFixedStepCount(); ++i) {
      auto* bc = m_ECS.GetComponent<BehaviorComponent>(m_GameObjects[i]->m_EntityHandle);

      if (!bc || !bc->behavior) {
        continue;
      }

      if (!bc->started) {
        bc->behavior->OnStart();
        bc->started = true;
      }

      bc->behavior->OnFixedUpdate();
    }
  }

  for (size_t i = 0; i < m_ObjCount; ++i) {
    auto* bc = m_ECS.GetComponent<BehaviorComponent>(m_GameObjects[i]->m_EntityHandle);

    if (!bc || !bc->behavior) {
      continue;
    }

    if (!bc->started) {
      bc->behavior->OnStart();
      bc->started = true;
    }

    bc->behavior->OnUpdate();
  }

  for (size_t i = 0; i < m_ObjCount; ++i) {
    auto* bc = m_ECS.GetComponent<BehaviorComponent>(m_GameObjects[i]->m_EntityHandle);

    if (!bc || !bc->behavior) {
      continue;
    }

    if (!bc->started) {
      bc->behavior->OnStart();
      bc->started = true;
    }

    bc->behavior->OnLateUpdate();
  }

  ECSSystems::UpdateLocalMatrices(m_ECS);
  ECSSystems::UpdateGlobalMatrices(m_ECS, m_RootEntity);
}

void Scene::OnFrameEnd() {
  using enum GameObject::Flags;

  ECSSystems::WalkHierarchy<ECSSystems::TreeWalkOrder::PRE>(
    m_ECS, m_RootEntity, [this](HierarchyComponent* hc, Entity e) -> bool {
      if (m_RootEntity == e) {
        return true;
      }

      if (m_ECS.GetComponent<GameObjectRef>(e)->object->HasObjectFlags(QUEUE_DESTROY)) {
        ECSSystems::WalkHierarchy<ECSSystems::TreeWalkOrder::PRE>(
          m_ECS, e, [this](HierarchyComponent* hc, Entity e) -> bool {
            auto obj_ref = m_ECS.GetComponent<GameObjectRef>(e)->object;
            obj_ref->EnableObjectFlags(QUEUE_DESTROY);

            auto bc = m_ECS.GetComponent<BehaviorComponent>(e);

            if (bc && bc->started) {
              if (obj_ref->HasObjectFlags(ENABLED_IN_HIERARCHY)) {
                bc->behavior->OnDisable();
              }
              bc->behavior->OnDestroy();
            }

            return true;
          }
        );

        return false;
      }

      return true;
    }
  );

  for (size_t i = 0; i < m_GameObjects.size();) {
    if (m_GameObjects[i]->HasObjectFlags(QUEUE_DESTROY)) {
      m_ECS.DeleteEntity(m_GameObjects[i]->m_EntityHandle);

      m_GameObjects[i] = std::move(m_GameObjects.back());
      m_GameObjects.pop_back();
    } else {
      ++i;
    }
  }

  m_ECS.FlushRemoval();

  for (auto& obj : m_GameObjects) {
    obj->DisableObjectFlags(NEW_OBJECT);
  }
}

void Scene::UnloadAllObjects() {
  using enum GameObject::Flags;

  for (auto& obj : m_GameObjects) {
    if (obj->HasObjectFlags(CONTAINS_BEHAVIOR)) {
      obj->GetComponent<BehaviorComponent>()->behavior->OnDestroy();
    }

    obj->GetComponent<GameObjectRef>()->object = nullptr;

    m_ECS.DeleteEntity(obj->m_EntityHandle);
  }

  m_ECS.FlushRemoval();
  m_GameObjects.clear();
}

void Scene::OnRender(Renderer2D& renderer) {
  ECSSystems::IssueDrawCommands(m_ECS, renderer);
}

ECSRegistry& Scene::GetECS() {
  return m_ECS;
}
}
