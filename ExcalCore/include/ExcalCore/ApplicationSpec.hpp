#pragma once

#include <cstdint>
#include <glm/ext/vector_int2.hpp>
#include <string>

namespace ExcalCore {

enum class FullscreenState : uint8_t {
  BORDERLESS,
  WINDOWED,
  FULLSCREEN,
};

struct WindowSpec {
  glm::ivec2 size = {1280, 720};
  FullscreenState fullscreenState = FullscreenState::BORDERLESS;
  bool allowResize = false;
  bool alwaysOnTop = false;
  bool startMaximized = true;
};

struct RendererSpec {
  glm::ivec2 size = {1280, 720};
  bool sameAsWindow = true;
  bool vsync = true;
  bool renderOutOfFocus = false;
};

struct AssetSpec {
  std::string directory = "Assets";
};

struct TimeSpec {
  float maxDelta = 0.2f;
  float fixedTimeStep = 0.02f;
};

struct InputSpec { };

struct EventSpec { };

struct ApplicationSpec {
  std::string name = "ExcalEngine Game";
  bool runOutOfFocus = false;

  WindowSpec windowSpec = {};
  RendererSpec rendererSpec = {};
  AssetSpec assetSpec = {};
  TimeSpec timeSpec = {};
  InputSpec inputSpec = {};
  EventSpec eventSpec = {};
};

}
