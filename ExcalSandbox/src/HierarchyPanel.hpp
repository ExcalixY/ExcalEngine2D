#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <unordered_map>

#include "ExcalCore/ECS/Entity.hpp"
#include "ExcalCore/Objects/GameObject.hpp"

class HierarchyPanel {
public:
  void Draw(std::span<const std::unique_ptr<ExcalCore::GameObject>> objects);

private:
  void DrawNode(ExcalCore::GameObject* obj);
  void DrawInspector();

  [[nodiscard]]
  ExcalCore::GameObject* Find(ExcalCore::Entity entity) const;
  [[nodiscard]]
  const char* NameOf(ExcalCore::Entity entity) const;

  std::unordered_map<uint64_t, ExcalCore::GameObject*> m_Lookup;
  ExcalCore::Entity m_Selected = ExcalCore::EMPTY_ENTITY;
  bool m_Visible = true;
};
