#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

struct EngineInitializer {
  EngineInitializer() {
    Debug::InitLogging("log.txt");
    Debug::Log("Initialized Logging System...");

    Debug::Log("Initializing SDL...");

    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
      Debug::ThrowError(
        Debug::LogCategory::CATEGORY_ENGINE,
        "Failed to initialize SDL: {}",
        SDL_GetError()
      );

      return;
    }

    Debug::Log("Initializing TTF Engine...");

    if (!TTF_Init()) {
      Debug::ThrowError(
        Debug::LogCategory::CATEGORY_ENGINE,
        "Failed to initialize TTF Engine: {}",
        SDL_GetError()
      );

      return;
    }
  }

  ~EngineInitializer() {
    Debug::Log("Destroying TTF Engine...");
    TTF_Quit();

    Debug::Log("Quitting SDL... baibai!!!");
    SDL_Quit();
  }
};

}
