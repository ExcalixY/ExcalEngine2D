#pragma once

#include <SDL3/SDL.h>

#include <functional>
#include <glm/vec2.hpp>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "Component.hpp"

namespace ExcalCore {

struct SpriteRenderer;

struct SpriteAnimation {
  std::vector<SDL_FRect> frames;
  float fps = 12.0f;
  bool loop = true;

  static SpriteAnimation FromGrid(
    const glm::vec2& cellSize,
    int columns,
    int firstFrame,
    int frameCount,
    float fps = 12.0f,
    bool loop = true
  );
};

struct SpriteAnimator : Component<SpriteAnimator>, IStartable, IUpdatable {
  static constexpr std::string_view Name = "SpriteAnimator";

  void OnStart() override;
  void OnUpdate(float dt) override;

  void AddAnimation(std::string name, SpriteAnimation animation);
  bool HasAnimation(std::string_view name) const;

  bool Play(std::string_view name, bool restart = false);
  void Stop();
  void Pause();
  void Resume();

  bool IsPlaying() const;
  bool IsFinished() const;

  const std::string& GetCurrentAnimation() const;
  size_t GetFrameIndex() const;

  float GetSpeed() const;
  void SetSpeed(float speed);

  std::function<void(const std::string&)> onFinished;

private:
  SpriteRenderer* FindRenderer();
  void ApplyFrame();

  std::map<std::string, SpriteAnimation, std::less<>> m_Animations;

  SpriteRenderer* m_Renderer = nullptr;
  const SpriteAnimation* m_Current = nullptr;
  std::string m_CurrentName;

  size_t m_FrameIndex = 0;
  float m_Timer = 0.0f;
  float m_Speed = 1.0f;
  bool m_Playing = false;
  bool m_Finished = false;
};

}
