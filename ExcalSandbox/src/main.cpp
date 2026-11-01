#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_rect.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <stdexcept>
#include <string>

#include "ExcalCore/Application.hpp"
#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Assets/TextureAsset.hpp"
#include "ExcalCore/ECS/Components/SpriteRenderer.hpp"
#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/Input/InputBindings.hpp"
#include "ExcalCore/Input/KeyCode.hpp"
#include "ExcalCore/Layers/Layer.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Objects/Scene.hpp"
#include "ExcalCore/Scripting/Behavior.hpp"
#include "ExcalCore/Systems/AssetManager.hpp"
#include "ExcalCore/Systems/InputSystem.hpp"
#include "ExcalCore/Systems/TimeSystem.hpp"

using namespace ExcalCore;

class PlayerMovement : public Behavior {
public:
  PlayerMovement(GameObject& obj, ECSRegistry& ecs, float speed)
      : Behavior(obj), m_Speed(speed) { }

  void OnUpdate() override {
    glm::vec2 direction = Input::GetAxis2D("Move");

    if (glm::length(direction) > 1.f) {
      direction = glm::normalize(direction);
    }

    m_Transform->pos += direction * m_Speed * Time::GetDeltaTime();
  }

private:
  float m_Speed;
};

class SandboxScene : public Scene {
public:
  SandboxScene() : Scene() { }

  void OnInit() override {
    std::string path = "Top-Down/Chips/ChipsA_Flat-64x72.png";

    GameObject* player = CreateObject("Player");

    player->GetComponent<Transform>()->pos = {640.f, 360.f};

    SpriteRenderer* sprite = player->AddComponent<SpriteRenderer>();
    sprite->texture = Assets::LoadAsset<TextureAsset>(path);
    sprite->source = SDL_FRect{0.f, 0.f, 64.f, 72.f};

    AttachBehavior<PlayerMovement>(player, 300.f);
  }
};

class SandboxLayer : public Layer {
public:
  explicit SandboxLayer(Application& app) : Layer("Sandbox") { }

  void OnAttach() override {
    m_Scene.OnInit();
  }

  void OnUpdate(float dt) override {
    m_Scene.OnFrameStart();
    m_Scene.OnUpdate(dt);
  }

  void OnRender(Renderer2D& renderer) override {
    m_Scene.OnRender(renderer);
    m_Scene.OnFrameEnd();
  }

private:
  ECSRegistry m_ECS;
  SandboxScene m_Scene;
};

int main(int argc, char* argv[]) {
  try {
    Application app({
      .name = "ExcalSandbox",
      .windowSpec = {
        .fullscreenState = FullscreenState::WINDOWED, .allowResize = false
      },
    });

    app.GetInput().BindInput(
      "Move",
      BindingAxis2D(
        ButtonSources{KeyCode::W, KeyCode::UP},
        ButtonSources{KeyCode::S, KeyCode::DOWN},
        ButtonSources{KeyCode::A, KeyCode::LEFT},
        ButtonSources{KeyCode::D, KeyCode::RIGHT}
      )
    );

    app.PushLayer<SandboxLayer>(app);
    app.Run();
  } catch (const std::runtime_error& e) {
    SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_ERROR, "ExcalEngine: Error", e.what(), nullptr
    );
    return 1;
  }

  return 0;
}
