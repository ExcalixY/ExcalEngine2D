#include "ExcalCore/Objects/Scene.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>

#include "ExcalCore/ECS/Components/BehaviorComponent.hpp"
#include "ExcalCore/ECS/Components/HierarchyComponent.hpp"
#include "ExcalCore/ECS/Components/ObjectDestroyComponent.hpp"
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
  ECSSystems::SetParentOfEntity(
    m_ECS, e, parent ? parent->m_EntityHandle : m_RootEntity
  );

  auto ptr = std::make_unique<GameObject>(e, name, m_ECS);
  ptr->SetEnabled(true);

  m_GameObjects.push_back(std::move(ptr));

  return m_GameObjects.back().get();
}

void Scene::OnFrameStart() {
  using enum GameObject::Flags;

  m_ObjCount = m_GameObjects.size();

  for (size_t i = 0; i < m_ObjCount; ++i) {
    GameObject* obj = m_GameObjects[i].get();

    if (
      obj->HasObjectFlags(QUEUE_ENABLE) &&
      !obj->HasObjectFlags(ENABLED, QUEUE_DISABLE)
    ) {
      obj->EnableObjectFlags(ENABLED);
      if (obj->HasObjectFlags(CONTAINS_BEHAVIOR)) {
        m_ECS.GetComponent<BehaviorComponent>(obj->m_EntityHandle)
          ->behavior->OnEnable();
      }
      m_ECS.AddComponent<ObjectEnabledComponent>(obj->m_EntityHandle);
    }

    obj->DisableObjectFlags(QUEUE_ENABLE, QUEUE_DISABLE);
  }
}

void Scene::OnUpdate(float dt) {
  auto snapshot = m_ECS.Snapshot<BehaviorComponent, ObjectEnabledComponent>();

  for (int i = 0; i < Time::GetFixedStepCount(); ++i) {
    for (Entity e : snapshot) {
      auto bc = m_ECS.GetComponent<BehaviorComponent>(e);

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

  for (Entity e : snapshot) {
    auto bc = m_ECS.GetComponent<BehaviorComponent>(e);

    if (!bc || !bc->behavior) {
      continue;
    }

    if (!bc->started) {
      bc->behavior->OnStart();
      bc->started = true;
    }

    bc->behavior->OnUpdate();
  }

  for (Entity e : snapshot) {
    auto bc = m_ECS.GetComponent<BehaviorComponent>(e);

    if (!bc || !bc->behavior) {
      continue;
    }

    bc->behavior->OnLateUpdate();
  }

  ECSSystems::UpdateLocalMatrices(m_ECS);
  ECSSystems::UpdateGlobalMatrices(m_ECS, m_RootEntity);
}

void Scene::OnFrameEnd() {
  using enum GameObject::Flags;

  for (size_t i = 0; i < m_ObjCount; ++i) {
    GameObject* obj = m_GameObjects[i].get();

    if (obj->HasObjectFlags(ENABLED, QUEUE_DISABLE)) {
      if (obj->HasObjectFlags(CONTAINS_BEHAVIOR)) {
        auto bc = obj->GetComponent<BehaviorComponent>();
        bc->behavior->OnDisable();
      }

      obj->RemoveComponent<ObjectEnabledComponent>();

      obj->DisableObjectFlags(ENABLED, QUEUE_DISABLE);
    }
  }

  // Object Destruction

  // Tag: post-order from each flagged object, so the pool order is children
  // first
  for (auto& obj : m_GameObjects) {
    if (obj->HasObjectFlags(QUEUE_DESTROY)) {
      ECSSystems::WalkHierarchy<ECSSystems::TreeWalkOrder::POST>(
        m_ECS, obj->m_EntityHandle, [this](auto, Entity e) {
          m_ECS.AddComponent<ObjectDestroyComponent>(
            e
          );  // no-op if already tagged
        }
      );
    }
  }

  // OnDestroy, children first, tree still intact
  for (Entity e : m_ECS.Snapshot<ObjectDestroyComponent>()) {
    if (
      auto* bc = m_ECS.GetComponent<BehaviorComponent>(e); bc && bc->behavior
    ) {
      bc->behavior->OnDestroy();
    }
  }

  // Unlink the top of each deleted subtree from its surviving parent
  for (Entity e : m_ECS.Snapshot<ObjectDestroyComponent>()) {
    Entity parent = m_ECS.GetComponent<HierarchyComponent>(e)->parent;
    if (!m_ECS.HasComponent<ObjectDestroyComponent>(parent)) {
      ECSSystems::DetachNodeFromOthers(m_ECS, e);
    }
  }

  // Delete all objects tagged with destruction
  std::erase_if(m_GameObjects, [this](const std::unique_ptr<GameObject>& obj) {
    if (m_ECS.HasComponent<ObjectDestroyComponent>(obj->m_EntityHandle)) {
      m_ECS.DeleteEntity(obj->m_EntityHandle);
      return true;
    }

    return false;
  });

  m_ECS.FlushRemoval();
}

void Scene::UnloadAllObjects() {
  for (auto& obj : m_GameObjects) {
    if (obj->HasObjectFlags(GameObject::Flags::CONTAINS_BEHAVIOR)) {
      auto bc = obj->GetComponent<BehaviorComponent>();
      bc->behavior->OnDestroy();
    }

    m_ECS.DeleteEntity(obj->m_EntityHandle);
  }

  m_GameObjects.clear();
}

void Scene::OnRender(Renderer2D& renderer) {
  ECSSystems::IssueDrawCommands(m_ECS, renderer);
}

ECSRegistry& Scene::GetECS() {
  return m_ECS;
}

}
