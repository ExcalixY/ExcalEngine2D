#include "ExcalCore/Systems/TimeSystem.hpp"

#include <algorithm>

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

TimeSystem::TimeSystem(const ApplicationSpec& spec)
    : m_MaxDeltaTime(spec.maxDelta) {
  Debug::Log("Initializing Time System...");

  m_LastTick = SDL_GetTicksNS();
}

TimeSystem::~TimeSystem() {
  Debug::Log("Destroying Time System...");
}

void TimeSystem::Tick() {
  Uint64 cur_tick = SDL_GetTicksNS();

  Uint64 tick_delta = cur_tick - m_LastTick;

  m_DeltaTimeUnscaled = std::min(
    static_cast<float>(tick_delta) / SDL_NS_PER_SECOND, m_MaxDeltaTime
  );
  m_DeltaTime = m_DeltaTimeUnscaled * m_TimeScale;

  m_TimeUnscaled += static_cast<double>(m_DeltaTimeUnscaled);
  m_Time += static_cast<double>(m_DeltaTime);

  m_LastTick = cur_tick;
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

void TimeSystem::Reset() {
  m_LastTick = SDL_GetTicksNS();

  m_DeltaTime = 0.f;
  m_DeltaTimeUnscaled = 0.f;

  m_Time = 0.0;
  m_TimeUnscaled = 0.0;

  m_TimeScale = 1.f;
}

}
