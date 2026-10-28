#pragma once

#include <glm/vec2.hpp>
#include <string>

namespace ExcalCore {

enum class FullscreenState {
  BORDERLESS,
  WINDOWED,
  FULLSCREEN,
};

struct WindowSpec {
  glm::ivec2 size = {1280, 720};
  FullscreenState fullscreenState = FullscreenState::BORDERLESS;
  bool vsync = false;
};

struct RendererSpec {
  glm::ivec2 size = {1280, 720};
  bool sameAsWindow = true;
  bool vsync = false;
};

struct ApplicationSpec {
  std::string name = "ExcalEngine Game";

  WindowSpec windowSpec = {};
  RendererSpec rendererSpec = {};

  float maxDelta = 0.02;
};

}
