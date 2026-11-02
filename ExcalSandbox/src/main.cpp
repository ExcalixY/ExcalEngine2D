#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_rect.h>

#include <cmath>
#include <format>
#include <glm/common.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <memory>
#include <numbers>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>

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
#include "ExcalCore/Utils/Logging.hpp"
#include "HierarchyPanel.hpp"

using namespace ExcalCore;

constexpr float MOVE_SPEED = 300.f;
constexpr float ROTATE_SPEED = 2.f;
constexpr float SCALE_SPEED = 1.f;
constexpr float MIN_SCALE = 0.1f;

constexpr int TOKENS_PER_RING = 10;
constexpr float TOKEN_SCALE = 0.3f;

class LoggedBehavior : public Behavior {
public:
  explicit LoggedBehavior(GameObject& obj) : Behavior(obj) { }

  void OnEnable() override {
    Debug::Log("[{}] OnEnable", m_GameObject.GetName());
  }

  void OnDisable() override {
    Debug::Log("[{}] OnDisable", m_GameObject.GetName());
  }

  void OnDestroy() override {
    Debug::Log("[{}] OnDestroy", m_GameObject.GetName());
  }
};

class TokenSpin : public LoggedBehavior {
public:
  TokenSpin(GameObject& obj, ECSRegistry& ecs, float speed)
      : LoggedBehavior(obj), m_Speed(speed) { }

  void OnUpdate() override {
    m_Transform->rot += m_Speed * Time::GetDeltaTime();
  }

private:
  float m_Speed;
};

// Reads "<prefix>Move", "<prefix>Rotate" and "<prefix>Scale"
class TransformController : public LoggedBehavior {
public:
  TransformController(GameObject& obj, ECSRegistry& ecs, std::string prefix)
      : LoggedBehavior(obj), m_Prefix(std::move(prefix)) { }

  void OnUpdate() override {
    float dt = Time::GetDeltaTime();

    glm::vec2 direction = Input::GetAxis2D(m_Prefix + "Move");
    if (glm::length(direction) > 1.f) {
      direction = glm::normalize(direction);
    }
    m_Transform->pos += direction * MOVE_SPEED * dt;

    m_Transform->rot += Input::GetAxis(m_Prefix + "Rotate") * ROTATE_SPEED * dt;

    float growth =
      std::exp(Input::GetAxis(m_Prefix + "Scale") * SCALE_SPEED * dt);
    m_Transform->scale =
      glm::max(m_Transform->scale * growth, glm::vec2(MIN_SCALE));
  }

private:
  std::string m_Prefix;
};

class SandboxScene : public Scene {
public:
  SandboxScene() : Scene() { }

  void OnInit() override {
    GameObject* parent = CreateObject("Parent");
    parent->GetComponent<Transform>()->pos = {640.f, 360.f};

    SpriteRenderer* parent_sprite = parent->AddComponent<SpriteRenderer>();
    parent_sprite->texture =
      Assets::LoadAsset<TextureAsset>("Top-Down/Cards/Card_Back-88x124.png");
    parent_sprite->source = SDL_FRect{0.f, 0.f, 88.f, 124.f};

    AttachBehavior<TransformController>(parent, "Parent");

    GameObject* child = CreateObject("Child", parent);
    child->GetComponent<Transform>()->pos = {150.f, 0.f};

    SpriteRenderer* child_sprite = child->AddComponent<SpriteRenderer>();
    child_sprite->texture =
      Assets::LoadAsset<TextureAsset>("Top-Down/Cards/Hearts-88x124.png");
    child_sprite->source = SDL_FRect{0.f, 0.f, 88.f, 124.f};

    AttachBehavior<TransformController>(child, "Child");

    SpawnTokenRing(parent, 1, 110.f);
    SpawnTokenRing(child, TOKENS_PER_RING + 1, 80.f);
  }

  [[nodiscard]]
  std::span<const std::unique_ptr<GameObject>> GetObjects() const {
    return m_GameObjects;
  }

private:
  void SpawnTokenRing(GameObject* parent, int first_number, float radius) {
    auto texture =
      Assets::LoadAsset<TextureAsset>("Top-Down/Chips/ChipsA_Flat-64x72.png");

    for (int i = 0; i < TOKENS_PER_RING; ++i) {
      float angle = static_cast<float>(i) / TOKENS_PER_RING * 2.f *
                    std::numbers::pi_v<float>;

      GameObject* token =
        CreateObject(std::format("Token {}", first_number + i), parent);

      Transform* transform = token->GetComponent<Transform>();
      transform->pos = glm::vec2{std::cos(angle), std::sin(angle)} * radius;
      transform->scale = {TOKEN_SCALE, TOKEN_SCALE};

      SpriteRenderer* sprite = token->AddComponent<SpriteRenderer>();
      sprite->texture = texture;
      sprite->source = SDL_FRect{0.f, 0.f, 64.f, 72.f};
      sprite->layer = 1;

      AttachBehavior<TokenSpin>(token, i % 2 == 0 ? 3.f : -3.f);
    }
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
    m_HierarchyPanel.Draw(m_Scene.GetObjects());
  }

  void OnFrameEnd() override {
    m_Scene.OnFrameEnd();
  }

private:
  ECSRegistry m_ECS;
  SandboxScene m_Scene;
  HierarchyPanel m_HierarchyPanel;
};

int main(int argc, char* argv[]) {
  try {
    Application app({
      .name = "ExcalSandbox",
      .windowSpec = {
        .fullscreenState = FullscreenState::WINDOWED, .allowResize = false
      },
    });

    InputSystem& input = app.GetInput();

    input.BindInput(
      "ParentMove",
      BindingAxis2D(
        ButtonSources{KeyCode::W},
        ButtonSources{KeyCode::S},
        ButtonSources{KeyCode::A},
        ButtonSources{KeyCode::D}
      )
    );
    input.BindInput(
      "ParentRotate",
      BindingAxis(ButtonSources{KeyCode::Q}, ButtonSources{KeyCode::E})
    );
    input.BindInput(
      "ParentScale",
      BindingAxis(ButtonSources{KeyCode::Z}, ButtonSources{KeyCode::X})
    );

    input.BindInput(
      "ChildMove",
      BindingAxis2D(
        ButtonSources{KeyCode::UP},
        ButtonSources{KeyCode::DOWN},
        ButtonSources{KeyCode::LEFT},
        ButtonSources{KeyCode::RIGHT}
      )
    );
    input.BindInput(
      "ChildRotate",
      BindingAxis(ButtonSources{KeyCode::COMMA}, ButtonSources{KeyCode::PERIOD})
    );
    input.BindInput(
      "ChildScale",
      BindingAxis(ButtonSources{KeyCode::MINUS}, ButtonSources{KeyCode::EQUALS})
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
