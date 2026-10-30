#include <SDL3/SDL.h>

#include <ExcalCore/Application.hpp>
#include <ExcalCore/ECS/Components/SpriteRenderer.hpp>
#include <ExcalCore/ECS/Components/Transform.hpp>
#include <ExcalCore/ECS/ECSRegistry.hpp>
#include <ExcalCore/ECS/Systems/SpriteRenderer.hpp>
#include <ExcalCore/ECS/Systems/Transform.hpp>
#include <ExcalCore/Utils/Logging.hpp>
#include <glm/geometric.hpp>
#include <stdexcept>
#include <string>

using namespace ExcalCore;

struct PlayerController : Component {
  float speed = 300.f;
};

namespace SandboxSystems {

void MovePlayers(ECSRegistry& ecs, const InputSystem& input, float dt) {
  glm::vec2 direction = input.GetAxis2D("Move");

  if (glm::length(direction) > 1.f) {
    direction = glm::normalize(direction);
  }

  ecs.Each<Transform, PlayerController>(
    [&](Entity, Transform* t, PlayerController* player) {
      t->pos += direction * player->speed * dt;
    }
  );
}

}

class SandboxLayer : public Layer {
public:
  explicit SandboxLayer(Application& app) : Layer("Sandbox"), m_App(app) { }

  void OnAttach() override {
    std::string path = std::string(SDL_GetBasePath()) +
                       "Assets/Top-Down/Chips/ChipsA_Flat-64x72.png";

    Entity token = m_ECS.NewEntity();

    m_ECS.AddComponent<Transform>(token)->pos = {640.f, 360.f};
    m_ECS.AddComponent<WorldTransform>(token);
    m_ECS.AddComponent<PlayerController>(token);

    SpriteRenderer* sprite = m_ECS.AddComponent<SpriteRenderer>(token);
    sprite->texture = m_App.GetAssets().LoadAsset<TextureAsset>(path);
    sprite->source = SDL_FRect{0.f, 0.f, 64.f, 72.f};
  }

  void OnUpdate(float dt) override {
    SandboxSystems::MovePlayers(m_ECS, m_App.GetInput(), dt);

    ECSSystems::UpdateLocalMatrices(m_ECS);
    ECSSystems::UpdateGlobalMatrices(m_ECS);
  }

  void OnRender(Renderer2D& renderer) override {
    ECSSystems::IssueDrawCommands(m_ECS, renderer);
  }

private:
  Application& m_App;
  ECSRegistry m_ECS;
};

int main(int argc, char* argv[]) {
  try {
    Application app({
      .name = "ExcalSandbox",
      .windowSpec = {.fullscreenState = FullscreenState::WINDOWED},
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
