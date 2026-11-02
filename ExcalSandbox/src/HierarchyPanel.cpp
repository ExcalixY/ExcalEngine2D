#include "HierarchyPanel.hpp"

#include <cxxabi.h>
#include <imgui.h>

#include <cstdint>
#include <cstdlib>
#include <format>
#include <memory>
#include <span>
#include <string>
#include <typeinfo>

#include "ExcalCore/ECS/Components/BehaviorComponent.hpp"
#include "ExcalCore/ECS/Components/HierarchyComponent.hpp"
#include "ExcalCore/ECS/Components/ObjectEnabledComponent.hpp"
#include "ExcalCore/ECS/Components/SpriteRenderer.hpp"
#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/Entity.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Scripting/Behavior.hpp"

using namespace ExcalCore;

namespace {

uint64_t Key(Entity entity) {
  return (static_cast<uint64_t>(entity.id) << 32) | entity.gen;
}

std::string Demangle(const char* name) {
  int status = 0;
  char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
  std::string result = status == 0 ? demangled : name;
  std::free(demangled);
  return result;
}

}

void HierarchyPanel::Draw(
  std::span<const std::unique_ptr<GameObject>> objects
) {
  if (ImGui::IsKeyPressed(ImGuiKey_F2, false)) {
    m_Visible = !m_Visible;
  }

  if (!m_Visible) {
    return;
  }

  m_Lookup.clear();
  for (const auto& obj : objects) {
    m_Lookup[Key(obj->GetHandle())] = obj.get();
  }

  ImGui::SetNextWindowSize({760.f, 520.f}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Scene Hierarchy (F2)", &m_Visible)) {
    ImGui::End();
    return;
  }

  ImGui::BeginChild(
    "Tree",
    {ImGui::GetContentRegionAvail().x * 0.5f, 0.f},
    ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX
  );

  // The scene root is not a GameObject, so its children are the objects whose
  // parent can't be found. The first of them has no previous sibling.
  for (const auto& obj : objects) {
    auto* hc = obj->GetComponent<HierarchyComponent>();

    if (hc && !Find(hc->parent) && hc->prevSibling == EMPTY_ENTITY) {
      for (GameObject* it = obj.get(); it;
           it = Find(it->GetComponent<HierarchyComponent>()->nextSibling)) {
        DrawNode(it);
      }
      break;
    }
  }

  ImGui::EndChild();
  ImGui::SameLine();

  ImGui::BeginChild("Inspector", {0.f, 0.f}, ImGuiChildFlags_Borders);
  DrawInspector();
  ImGui::EndChild();

  ImGui::End();
}

void HierarchyPanel::DrawNode(GameObject* obj) {
  auto* hc = obj->GetComponent<HierarchyComponent>();
  Entity entity = obj->GetHandle();
  bool has_children = Find(hc->firstChild) != nullptr;

  ImGui::PushID(obj);

  ImGuiStorage* storage = ImGui::GetStateStorage();
  ImGuiID open_id = ImGui::GetID("open");
  bool open = storage->GetBool(open_id, false);

  float arrow_size = ImGui::GetFrameHeight();
  float row_start = ImGui::GetCursorPosX();
  float row_width = ImGui::GetContentRegionAvail().x;
  float label_width = row_width - arrow_size - ImGui::GetStyle().ItemSpacing.x;

  std::string label = std::format("{} GameObject", obj->GetName());
  bool enabled = obj->HasComponent<ObjectEnabledComponent>();

  if (!enabled) {
    ImGui::PushStyleColor(
      ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)
    );
  }

  ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, {0.f, 0.5f});
  if (
    ImGui::Selectable(
      label.c_str(),
      m_Selected == entity,
      ImGuiSelectableFlags_AllowOverlap,
      {label_width, arrow_size}
    )
  ) {
    m_Selected = entity;
  }
  ImGui::PopStyleVar();

  if (!enabled) {
    ImGui::PopStyleColor();
  }

  if (ImGui::BeginPopupContextItem("##context")) {
    if (ImGui::MenuItem(enabled ? "Disable" : "Enable")) {
      obj->SetEnabled(!enabled);
    }
    if (ImGui::MenuItem("Delete")) {
      obj->Destroy();
    }
    ImGui::EndPopup();
  }

  if (has_children) {
    ImGui::SameLine(row_start + row_width - arrow_size);
    if (ImGui::ArrowButton("##expand", open ? ImGuiDir_Down : ImGuiDir_Right)) {
      open = !open;
      storage->SetBool(open_id, open);
    }
  }

  if (has_children && open) {
    ImGui::Indent();
    for (GameObject* child = Find(hc->firstChild); child;
         child = Find(child->GetComponent<HierarchyComponent>()->nextSibling)) {
      DrawNode(child);
    }
    ImGui::Unindent();
  }

  ImGui::PopID();
}

void HierarchyPanel::DrawInspector() {
  GameObject* obj = Find(m_Selected);

  if (!obj) {
    ImGui::TextDisabled("Select a GameObject to inspect it.");
    return;
  }

  Entity entity = obj->GetHandle();
  ImGui::TextUnformatted(obj->GetName().c_str());
  ImGui::TextDisabled("Entity [id: %u, gen: %u]", entity.id, entity.gen);
  ImGui::Separator();

  constexpr ImGuiTreeNodeFlags open_flags = ImGuiTreeNodeFlags_DefaultOpen;

  if (auto* t = obj->GetComponent<Transform>()) {
    if (ImGui::CollapsingHeader("Transform", open_flags)) {
      ImGui::Text("Position  (%.1f, %.1f)", t->pos.x, t->pos.y);
      ImGui::Text("Rotation  %.1f", t->rot);
      ImGui::Text("Scale     (%.2f, %.2f)", t->scale.x, t->scale.y);
    }
  }

  if (auto* wt = obj->GetComponent<WorldTransform>()) {
    if (ImGui::CollapsingHeader("WorldTransform", open_flags)) {
      ImGui::Text(
        "Translation  (%.1f, %.1f)", wt->worldMatrix[2].x, wt->worldMatrix[2].y
      );
    }
  }

  if (auto* hc = obj->GetComponent<HierarchyComponent>()) {
    if (ImGui::CollapsingHeader("HierarchyComponent", open_flags)) {
      ImGui::Text("Parent       %s", NameOf(hc->parent));
      ImGui::Text("FirstChild   %s", NameOf(hc->firstChild));
      ImGui::Text("LastChild    %s", NameOf(hc->lastChild));
      ImGui::Text("PrevSibling  %s", NameOf(hc->prevSibling));
      ImGui::Text("NextSibling  %s", NameOf(hc->nextSibling));
    }
  }

  if (auto* sprite = obj->GetComponent<SpriteRenderer>()) {
    if (ImGui::CollapsingHeader("SpriteRenderer", open_flags)) {
      ImGui::Text("Texture  %s", sprite->texture ? "loaded" : "none");
      ImGui::Text("Layer    %d", sprite->layer);
      ImGui::Text(
        "Color    (%.2f, %.2f, %.2f, %.2f)",
        sprite->color.r,
        sprite->color.g,
        sprite->color.b,
        sprite->color.a
      );
    }
  }

  if (auto* bc = obj->GetComponent<BehaviorComponent>()) {
    if (ImGui::CollapsingHeader("BehaviorComponent", open_flags)) {
      std::string type =
        bc->behavior ? Demangle(typeid(*bc->behavior).name()) : "none";
      ImGui::Text("Behavior  %s", type.c_str());
      ImGui::Text("Started   %s", bc->started ? "yes" : "no");
    }
  }

  if (obj->GetComponent<ObjectEnabledComponent>()) {
    ImGui::CollapsingHeader("ObjectEnabledComponent", ImGuiTreeNodeFlags_Leaf);
  }
}

GameObject* HierarchyPanel::Find(Entity entity) const {
  if (entity == EMPTY_ENTITY) {
    return nullptr;
  }

  auto it = m_Lookup.find(Key(entity));
  return it != m_Lookup.end() ? it->second : nullptr;
}

const char* HierarchyPanel::NameOf(Entity entity) const {
  if (entity == EMPTY_ENTITY) {
    return "-";
  }

  GameObject* obj = Find(entity);
  return obj ? obj->GetName().c_str() : "Scene root";
}
