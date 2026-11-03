#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_video.h>

#include <algorithm>
#include <cmath>
#include <format>
#include <glm/common.hpp>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>

#include "ExcalCore/Application.hpp"
#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Assets/TextureAsset.hpp"
#include "ExcalCore/ECS/Components/SpriteRenderer.hpp"
#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/Input/InputBindings.hpp"
#include "ExcalCore/Input/KeyCode.hpp"
#include "ExcalCore/Layers/Layer.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Objects/Scene.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Rendering/Window.hpp"
#include "ExcalCore/Scripting/Behavior.hpp"
#include "ExcalCore/Systems/AssetManager.hpp"
#include "ExcalCore/Systems/InputSystem.hpp"
#include "ExcalCore/Systems/TimeSystem.hpp"
#include "ExcalCore/Utils/Logging.hpp"

using namespace ExcalCore;

constexpr glm::vec2 SCREEN_SIZE = {1280.f, 720.f};

constexpr glm::vec2 PADDLE_SIZE = {88.f, 124.f};
constexpr float PADDLE_SPEED = 700.f;

constexpr glm::vec2 CHIP_SIZE = {64.f, 72.f};
constexpr float CHIP_SCALE = 0.75f;
constexpr float CHIP_MIN_SPEED = 180.f;
constexpr float CHIP_MAX_SPEED = 380.f;
constexpr float SPAWN_INTERVAL = 0.5f;

constexpr int START_LIVES = 5;

namespace {

struct GameState {
  int score = 0;
  int lives = START_LIVES;
  bool dirty = true;
};

GameState s_State;
std::mt19937 s_Rng(std::random_device{}());

bool Overlaps(glm::vec2 a_pos, glm::vec2 a_size, glm::vec2 b_pos, glm::vec2 b_size) {
  glm::vec2 delta = glm::abs(a_pos - b_pos);
  glm::vec2 reach = (a_size + b_size) * 0.5f;
  return delta.x < reach.x && delta.y < reach.y;
}

void UpdateTitle() {
  if (!s_State.dirty) {
    return;
  }

  std::string title = s_State.lives > 0
                        ? std::format("Chip Catch | Score {} | Lives {}", s_State.score, s_State.lives)
                        : std::format("Chip Catch | GAME OVER | Score {} | Press R to restart", s_State.score);

  SDL_SetWindowTitle(Window::GetInstance()->GetWindowRaw(), title.c_str());
  s_State.dirty = false;
}

}

class Paddle : public Behavior {
public:
  Paddle(GameObject& obj, Scene& scene) : Behavior(obj, scene) { }

  void OnUpdate() override {
    float axis = Input::GetAxis("Move");
    float half = PADDLE_SIZE.x * 0.5f;

    transform->pos.x += axis * PADDLE_SPEED * Time::GetDeltaTime();
    transform->pos.x = std::clamp(transform->pos.x, half, SCREEN_SIZE.x - half);
  }
};

class Chip : public Behavior {
public:
  Chip(GameObject& obj, Scene& scene, GameObject* paddle, float speed)
      : Behavior(obj, scene), m_Paddle(paddle), m_Speed(speed) { }

  void OnUpdate() override {
    transform->pos.y += m_Speed * Time::GetDeltaTime();

    if (s_State.lives <= 0) {
      return;
    }

    glm::vec2 paddle_pos = m_Paddle->GetComponent<Transform>()->pos;

    if (Overlaps(transform->pos, CHIP_SIZE * CHIP_SCALE, paddle_pos, PADDLE_SIZE)) {
      ++s_State.score;
      s_State.dirty = true;
      gameObject.Destroy();
    } else if (transform->pos.y > SCREEN_SIZE.y + CHIP_SIZE.y) {
      --s_State.lives;
      s_State.dirty = true;
      gameObject.Destroy();
    }
  }

  void OnDestroy() override {
    ++s_Destroyed;
  }

  static inline int s_Destroyed = 0;

private:
  GameObject* m_Paddle;
  float m_Speed;
};

class ChipCatchScene : public Scene {
public:
  void OnInit() override {
    m_ChipTexture = Assets::LoadAsset<TextureAsset>("Top-Down/Chips/ChipsA_Flat-64x72.png");
    auto card_texture = Assets::LoadAsset<TextureAsset>("Top-Down/Cards/Card_Back-88x124.png");

    m_Paddle = CreateObject("Paddle");
    m_Paddle->GetComponent<Transform>()->pos = {SCREEN_SIZE.x * 0.5f, SCREEN_SIZE.y - PADDLE_SIZE.y * 0.5f - 16.f};

    auto* sprite = m_Paddle->AddComponent<SpriteRenderer>();
    sprite->texture = card_texture;
    sprite->source = SDL_FRect{0.f, 0.f, PADDLE_SIZE.x, PADDLE_SIZE.y};
    sprite->drawOrder = 1;

    AttachBehavior<Paddle>(m_Paddle);

    m_Chips = CreateObject("Chips");
  }

  void OnUpdate(float dt) override {
    if (s_State.lives > 0) {
      m_SpawnTimer -= dt;

      if (m_SpawnTimer <= 0.f) {
        m_SpawnTimer += SPAWN_INTERVAL;
        SpawnChip();
      }
    } else if (Input::IsKeyPressed(KeyCode::R)) {
      Restart();
    }

    Scene::OnUpdate(dt);
    UpdateTitle();
  }

private:
  void SpawnChip() {
    std::uniform_real_distribution<float> rand_x(CHIP_SIZE.x, SCREEN_SIZE.x - CHIP_SIZE.x);
    std::uniform_real_distribution<float> rand_speed(CHIP_MIN_SPEED, CHIP_MAX_SPEED);
    std::uniform_int_distribution<int> rand_cell(0, 9);

    int cell = rand_cell(s_Rng);

    GameObject* chip = CreateObject(std::format("Chip {}", m_Spawned++), m_Chips);

    auto* transform = chip->GetComponent<Transform>();
    transform->pos = {rand_x(s_Rng), -CHIP_SIZE.y};
    transform->scale = {CHIP_SCALE, CHIP_SCALE};

    auto* sprite = chip->AddComponent<SpriteRenderer>();
    sprite->texture = m_ChipTexture;
    sprite->source = SDL_FRect{(cell % 5) * CHIP_SIZE.x, (cell / 5) * CHIP_SIZE.y, CHIP_SIZE.x, CHIP_SIZE.y};

    AttachBehavior<Chip>(chip, m_Paddle, rand_speed(s_Rng));
  }

  // Destroys the whole "Chips" subtree at once, then rebuilds the parent
  void Restart() {
    Debug::Log("Restart: {} chips destroyed so far", Chip::s_Destroyed);

    DestroyObject(m_Chips);
    m_Chips = CreateObject("Chips");

    s_State = {};
    m_SpawnTimer = 0.f;
  }

  std::shared_ptr<TextureAsset> m_ChipTexture;
  GameObject* m_Paddle = nullptr;
  GameObject* m_Chips = nullptr;

  float m_SpawnTimer = 0.f;
  int m_Spawned = 0;
};

class ChipCatchLayer : public Layer {
public:
  ChipCatchLayer() : Layer("ChipCatch") { }

  void OnAttach() override {
    m_Scene.OnInit();
  }

  void OnUpdate(float dt) override {
    m_Scene.OnFrameStart();
    m_Scene.OnUpdate(dt);
  }

  void OnRender(Renderer2D& renderer) override {
    m_Scene.OnRender(renderer);
  }

  void OnFrameEnd() override {
    m_Scene.OnFrameEnd();
  }

private:
  ChipCatchScene m_Scene;
};

int main() {
  try {
    Application app({
      .name = "Chip Catch",
      .windowSpec = {.fullscreenState = FullscreenState::WINDOWED, .allowResize = false, .startMaximized = false},
    });

    app.GetInput().BindInput(
      "Move", BindingAxis(ButtonSources{KeyCode::A, KeyCode::LEFT}, ButtonSources{KeyCode::D, KeyCode::RIGHT})
    );

    app.PushLayer<ChipCatchLayer>();
    app.Run();
  } catch (const std::runtime_error& e) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ExcalEngine: Error", e.what(), nullptr);
    return 1;
  }

  return 0;
}
