#pragma once

#include <SDL3/SDL.h>

#include "ExcalCore/ApplicationSpec.hpp"

namespace ExcalCore {

class TimeSystem final {
public:
  explicit TimeSystem(const ApplicationSpec& spec);
  ~TimeSystem();

  void Tick();

  float GetDeltaTime() const;
  float GetDeltaTimeUnscaled() const;

  double GetTime() const;
  double GetTimeUnscaled() const;

  float GetTimeScale() const;
  void SetTimeScale(float scale);

  void Reset();

private:
  Uint64 m_LastTick = 0;

  float m_DeltaTime = 0.f;
  float m_DeltaTimeUnscaled = 0.f;

  double m_Time = 0.0;
  double m_TimeUnscaled = 0.0;

  float m_TimeScale = 1.f;

  float m_MaxDeltaTime;
};

}
