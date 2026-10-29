#include "GameItems.hpp"

void GameScene::OnInit() {
  auto& obj = m_GameObjects.emplace_back();
}

void GameLayer::OnAttach() {
  m_Scene.OnInit();
}

void GameLayer::OnUpdate(float dt) {
  m_Scene.OnUpdate(dt);
}

void GameLayer::OnRender(ExcalCore::Renderer2D& renderer) {
  m_Scene.OnRender(renderer);
}
