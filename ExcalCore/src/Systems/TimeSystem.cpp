#include "ExcalCore/Systems/TimeSystem.hpp"

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>

#include <algorithm>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

TimeSystem* TimeSystem::instance = nullptr;

TimeSystem::TimeSystem(const ApplicationSpec& spec)
    : m_FixedTimeStep(spec.timeSpec.fixedTimeStep),
      m_MaxDeltaTime(spec.timeSpec.maxDelta) {
  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Attempted to initialize a second instance of TimeSystem."
    );
  }

  instance = this;
  Debug::Log("Initializing Time System...");

  m_LastTick = SDL_GetTicksNS();
}

TimeSystem::~TimeSystem() {
  Debug::Log("Destroying Time System...");
  instance = nullptr;
}

const TimeSystem* TimeSystem::GetInstance() {
  if (!instance) {
    Debug::ThrowError(Debug::CATEGORY_ENGINE, "TimeSystem not initialized...");
  }
  return instance;
}

void TimeSystem::Tick() {
  m_FixedStepCount = 0;

  Uint64 cur_tick = SDL_GetTicksNS();

  Uint64 tick_delta = cur_tick - m_LastTick;

  m_DeltaTimeUnscaled = std::min(
    static_cast<float>(tick_delta) / SDL_NS_PER_SECOND, m_MaxDeltaTime
  );
  m_DeltaTime = m_DeltaTimeUnscaled * m_TimeScale;

  m_TimeUnscaled += static_cast<double>(m_DeltaTimeUnscaled);
  m_Time += static_cast<double>(m_DeltaTime);

  m_LastTick = cur_tick;

  m_FixedTimer += m_DeltaTime;
  while (m_FixedTimer >= m_FixedTimeStep) {
    m_FixedTimer -= m_FixedTimeStep;
    ++m_FixedStepCount;
  }
}

float TimeSystem::GetDeltaTime() const {
  return m_DeltaTime;
}

float TimeSystem::GetDeltaTimeUnscaled() const {
  return m_DeltaTimeUnscaled;
}

double TimeSystem::GetTime() const {
  return m_Time;
}

double TimeSystem::GetTimeUnscaled() const {
  return m_TimeUnscaled;
}

float TimeSystem::GetTimeScale() const {
  return m_TimeScale;
}

void TimeSystem::SetTimeScale(float scale) {
  m_TimeScale = std::max(scale, 0.f);
}

float TimeSystem::GetFixedTimeStep() const {
  return m_FixedTimeStep;
}

int TimeSystem::GetFixedStepCount() const {
  return m_FixedStepCount;
}

void TimeSystem::Reset() {
  m_LastTick = SDL_GetTicksNS();

  m_DeltaTime = 0.f;
  m_DeltaTimeUnscaled = 0.f;

  m_Time = 0.0;
  m_TimeUnscaled = 0.0;

  m_FixedTimer = 0.0f;
  m_FixedStepCount = 0;
}

namespace Time {

float GetDeltaTime() {
  return TimeSystem::GetInstance()->GetDeltaTime();
}

float GetDeltaTimeUnscaled() {
  return TimeSystem::GetInstance()->GetDeltaTimeUnscaled();
}

double GetTime() {
  return TimeSystem::GetInstance()->GetTime();
}

double GetTimeUnscaled() {
  return TimeSystem::GetInstance()->GetTimeUnscaled();
}

float GetTimeScale() {
  return TimeSystem::GetInstance()->GetTimeScale();
}

float GetFixedTimeStep() {
  return TimeSystem::GetInstance()->GetFixedTimeStep();
}

int GetFixedStepCount() {
  return TimeSystem::GetInstance()->GetFixedStepCount();
}

}

}
