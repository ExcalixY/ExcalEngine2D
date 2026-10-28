#pragma once

#include "ExcalCore/Layers/Layer.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Objects/Scene.hpp"

class PlayerObject : public ExcalCore::GameObject {
public:
private:
};

class GameScene : public ExcalCore::Scene {
public:
  void OnInit() {
    auto& obj = m_GameObjects.emplace_back();
  }

private:
};

class GameLayer : public ExcalCore::Layer {
public:
  void OnAttach() override {
    m_Scene.OnInit();
  }

  void OnUpdate(float dt) override {
    m_Scene.OnUpdate(dt);
  }

  void OnRender(ExcalCore::Renderer2D& renderer) override {
    m_Scene.OnRender(renderer);
  }

private:
  GameScene m_Scene;
};
