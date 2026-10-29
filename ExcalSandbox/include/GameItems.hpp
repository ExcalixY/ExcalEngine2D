#pragma once

#include "ExcalCore/Layers/Layer.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Objects/Scene.hpp"

class GameScene : public ExcalCore::Scene {
public:
  void OnInit();

private:
};

class GameLayer : public ExcalCore::Layer {
public:
  void OnAttach() override;
  void OnUpdate(float dt) override;
  void OnRender(ExcalCore::Renderer2D& renderer) override;

private:
  GameScene m_Scene;
};
