#include "ExcalCore/Objects/Components/SpriteAnimator.hpp"

#include "ExcalCore/Objects/Components/SpriteRenderer.hpp"
#include "ExcalCore/Objects/GameObject.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

SpriteAnimation SpriteAnimation::FromGrid(
  const glm::vec2& cellSize,
  int columns,
  int firstFrame,
  int frameCount,
  float fps,
  bool loop
) {
  SpriteAnimation animation;
  animation.fps = fps;
  animation.loop = loop;
  animation.frames.reserve(frameCount);

  for (int i = 0; i < frameCount; ++i) {
    int index = firstFrame + i;
    animation.frames.push_back(
      {(index % columns) * cellSize.x,
       (index / columns) * cellSize.y,
       cellSize.x,
       cellSize.y}
    );
  }

  return animation;
}

void SpriteAnimator::OnStart() {
  ApplyFrame();
}

void SpriteAnimator::OnUpdate(float dt) {
  if (
    !IsEnabled() || !m_Playing || !m_Current || m_Current->frames.empty() ||
    m_Current->fps <= 0.0f
  ) {
    return;
  }

  float frame_duration = 1.0f / m_Current->fps;
  size_t last_frame = m_Current->frames.size() - 1;
  size_t previous_frame = m_FrameIndex;

  m_Timer += dt * m_Speed;
  while (m_Timer >= frame_duration) {
    m_Timer -= frame_duration;

    if (m_FrameIndex < last_frame) {
      ++m_FrameIndex;
    } else if (m_Current->loop) {
      m_FrameIndex = 0;
    } else {
      m_Playing = false;
      m_Finished = true;
      m_Timer = 0.0f;
      break;
    }
  }

  if (m_FrameIndex != previous_frame) {
    ApplyFrame();
  }

  if (m_Finished && onFinished) {
    onFinished(m_CurrentName);
  }
}

void SpriteAnimator::AddAnimation(std::string name, SpriteAnimation animation) {
  m_Animations.insert_or_assign(std::move(name), std::move(animation));
}

bool SpriteAnimator::HasAnimation(std::string_view name) const {
  return m_Animations.contains(name);
}

bool SpriteAnimator::Play(std::string_view name, bool restart) {
  auto it = m_Animations.find(name);
  if (it == m_Animations.end()) {
    Debug::LogWarning(
      Debug::CATEGORY_GAME, "SpriteAnimator has no animation '{}'", name
    );
    return false;
  }

  if (m_Current == &it->second && m_Playing && !restart) {
    return true;
  }

  m_Current = &it->second;
  m_CurrentName = it->first;
  m_FrameIndex = 0;
  m_Timer = 0.0f;
  m_Playing = true;
  m_Finished = false;

  ApplyFrame();
  return true;
}

void SpriteAnimator::Stop() {
  m_Playing = false;
  m_FrameIndex = 0;
  m_Timer = 0.0f;
  ApplyFrame();
}

void SpriteAnimator::Pause() {
  m_Playing = false;
}

void SpriteAnimator::Resume() {
  if (m_Current && !m_Finished) {
    m_Playing = true;
  }
}

bool SpriteAnimator::IsPlaying() const {
  return m_Playing;
}

bool SpriteAnimator::IsFinished() const {
  return m_Finished;
}

const std::string& SpriteAnimator::GetCurrentAnimation() const {
  return m_CurrentName;
}

size_t SpriteAnimator::GetFrameIndex() const {
  return m_FrameIndex;
}

float SpriteAnimator::GetSpeed() const {
  return m_Speed;
}

void SpriteAnimator::SetSpeed(float speed) {
  m_Speed = speed;
}

SpriteRenderer* SpriteAnimator::FindRenderer() {
  if (!m_Renderer && GetOwner()) {
    m_Renderer = GetOwner()->GetComponent<SpriteRenderer>();
  }
  return m_Renderer;
}

void SpriteAnimator::ApplyFrame() {
  SpriteRenderer* renderer = FindRenderer();
  if (!renderer || !m_Current || m_FrameIndex >= m_Current->frames.size()) {
    return;
  }

  renderer->SetSourceRect(m_Current->frames[m_FrameIndex]);
}

}
