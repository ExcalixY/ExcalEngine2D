#pragma once

#include <SDL3/SDL_stdinc.h>

#include "ExcalCore/ApplicationSpec.hpp"

namespace ExcalCore {

class TimeSystem final {
public:
  explicit TimeSystem(const ApplicationSpec& spec);
  ~TimeSystem();

  TimeSystem(const TimeSystem&) = delete;
  TimeSystem& operator=(const TimeSystem&) = delete;

  [[nodiscard]]
  static const TimeSystem* GetInstance();

  void Tick();

  [[nodiscard]]
  float GetDeltaTime() const;
  [[nodiscard]]
  float GetDeltaTimeUnscaled() const;

  [[nodiscard]]
  double GetTime() const;
  [[nodiscard]]
  double GetTimeUnscaled() const;

  [[nodiscard]]
  float GetTimeScale() const;
  void SetTimeScale(float scale);

  [[nodiscard]]
  float GetFixedTimeStep() const;
  [[nodiscard]]
  int GetFixedStepCount() const;

  void Reset();

private:
  static TimeSystem* instance;

  Uint64 m_LastTick = 0;

  float m_DeltaTime = 0.f;
  float m_DeltaTimeUnscaled = 0.f;

  double m_Time = 0.0;
  double m_TimeUnscaled = 0.0;

  float m_TimeScale = 1.f;

  float m_FixedTimeStep = 0.02f;
  float m_FixedTimer = 0.0f;
  int m_FixedStepCount = 0;

  float m_MaxDeltaTime;
};

namespace Time {

[[nodiscard]]
float GetDeltaTime();
[[nodiscard]]
float GetDeltaTimeUnscaled();
[[nodiscard]]
double GetTime();
[[nodiscard]]
double GetTimeUnscaled();
[[nodiscard]]
float GetTimeScale();
[[nodiscard]]
float GetFixedTimeStep();
[[nodiscard]]
int GetFixedStepCount();

}

}
