#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include <cmath>
#include <cstdlib>
#include <format>
#include <glm/ext/vector_float2.hpp>
#include <numbers>
#include <random>
#include <stdexcept>
#include <string>

#include "ExcalCore/Application.hpp"
#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Assets/TextureAsset.hpp"
#include "ExcalCore/ECS/Components/Component.hpp"
#include "ExcalCore/ECS/Components/SpriteRenderer.hpp"
#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"
#include "ExcalCore/Layers/Layer.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Objects/Scene.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Rendering/Window.hpp"
#include "ExcalCore/Systems/AssetManager.hpp"
#include "ExcalCore/Systems/TimeSystem.hpp"
#include "ExcalCore/Utils/Logging.hpp"

using namespace ExcalCore;

constexpr int INSTANCE_COUNT = 100000;

constexpr float TOKEN_SCALE = 0.08f;
constexpr glm::vec2 TOKEN_SIZE = {64.f, 72.f};
constexpr float MIN_SPEED = 100.f;
constexpr float MAX_SPEED = 400.f;

namespace {

glm::vec2 s_ScreenSize = {1280.f, 720.f};

void UpdateScreenSize() {
  int width = 0;
  int height = 0;
  SDL_GetCurrentRenderOutputSize(
    Renderer2D::GetInstance()->GetRendererRaw(), &width, &height
  );
  s_ScreenSize = {static_cast<float>(width), static_cast<float>(height)};
}

void Bounce(glm::vec2& pos, glm::vec2& velocity, float dt) {
  pos += velocity * dt;

  glm::vec2 half = TOKEN_SIZE * TOKEN_SCALE * 0.5f;

  for (int axis = 0; axis < 2; ++axis) {
    if (pos[axis] - half[axis] < 0.f) {
      pos[axis] = half[axis];
      velocity[axis] = std::abs(velocity[axis]);
    } else if (pos[axis] + half[axis] > s_ScreenSize[axis]) {
      pos[axis] = s_ScreenSize[axis] - half[axis];
      velocity[axis] = -std::abs(velocity[axis]);
    }
  }
}

}

struct Bouncer : Component {
  glm::vec2 velocity = {0.f, 0.f};
};

void UpdateBouncers(ECSRegistry& ecs, float dt) {
  ecs.Each<Transform, Bouncer>([dt](Entity, Transform* t, Bouncer* b) {
    Bounce(t->pos, b->velocity, dt);
  });
}

class StressScene : public Scene {
public:
  StressScene(ECSRegistry& ecs, int instance_count)
      : Scene(), m_InstanceCount(instance_count) { }

  void OnInit() override {
    auto texture =
      Assets::LoadAsset<TextureAsset>("Top-Down/Chips/ChipsA_Flat-64x72.png");

    std::mt19937 rng(1234);
    std::uniform_real_distribution<float> rand_x(0.f, s_ScreenSize.x);
    std::uniform_real_distribution<float> rand_y(0.f, s_ScreenSize.y);
    std::uniform_real_distribution<float> rand_angle(
      0.f, 2.f * std::numbers::pi_v<float>
    );
    std::uniform_real_distribution<float> rand_speed(MIN_SPEED, MAX_SPEED);

    Uint64 start = SDL_GetTicksNS();

    for (int i = 0; i < m_InstanceCount; ++i) {
      GameObject* token = CreateObject("Token");

      Transform* transform = token->GetComponent<Transform>();
      transform->pos = {rand_x(rng), rand_y(rng)};
      transform->scale = {TOKEN_SCALE, TOKEN_SCALE};

      SpriteRenderer* sprite = token->AddComponent<SpriteRenderer>();
      sprite->texture = texture;
      sprite->source = SDL_FRect{0.f, 0.f, TOKEN_SIZE.x, TOKEN_SIZE.y};

      float angle = rand_angle(rng);
      float speed = rand_speed(rng);
      Bouncer* bouncer = token->AddComponent<Bouncer>();
      bouncer->velocity = glm::vec2{std::cos(angle), std::sin(angle)} * speed;
    }

    Debug::Log(
      "Spawned {} Component tokens in {:.1f} ms",
      m_InstanceCount,
      static_cast<double>(SDL_GetTicksNS() - start) / 1e6
    );
  }

private:
  int m_InstanceCount;
};

class StressLayer : public Layer {
public:
  explicit StressLayer(int instance_count)
      : Layer("StressComponent"),
        m_Scene(m_ECS, instance_count),
        m_InstanceCount(instance_count) { }

  void OnAttach() override {
    UpdateScreenSize();
    m_Scene.OnInit();
    m_LastReport = SDL_GetTicksNS();
  }

  void OnUpdate(float dt) override {
    UpdateScreenSize();

    m_Scene.OnFrameStart();
    UpdateBouncers(m_ECS, Time::GetDeltaTime());
    m_Scene.OnUpdate(dt);

    ReportFps();
  }

  void OnRender(Renderer2D& renderer) override {
    m_Scene.OnRender(renderer);
    m_Scene.OnFrameEnd();
  }

private:
  void ReportFps() {
    ++m_Frames;

    Uint64 now = SDL_GetTicksNS();
    Uint64 elapsed = now - m_LastReport;

    if (elapsed < SDL_NS_PER_SECOND) {
      return;
    }

    double seconds = static_cast<double>(elapsed) / SDL_NS_PER_SECOND;
    double fps = m_Frames / seconds;
    std::string report = std::format(
      "Component | {} tokens | {:.1f} FPS | {:.2f} ms",
      m_InstanceCount,
      fps,
      1000.0 / fps
    );

    Debug::Log("{}", report);
    SDL_SetWindowTitle(Window::GetInstance()->GetWindowRaw(), report.c_str());

    m_Frames = 0;
    m_LastReport = now;
  }

  ECSRegistry m_ECS;
  StressScene m_Scene;

  int m_InstanceCount;
  int m_Frames = 0;
  Uint64 m_LastReport = 0;
};

int main(int argc, char* argv[]) {
  int instance_count = argc > 1 ? std::atoi(argv[1]) : INSTANCE_COUNT;

  try {
    Application app({
      .name = "ExcalStress: Component",
      .windowSpec = {.fullscreenState = FullscreenState::WINDOWED},
      .rendererSpec = {.vsync = true},
    });

    app.PushLayer<StressLayer>(instance_count);
    app.Run();
  } catch (const std::runtime_error& e) {
    SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_ERROR, "ExcalEngine: Error", e.what(), nullptr
    );
    return 1;
  }

  return 0;
}
