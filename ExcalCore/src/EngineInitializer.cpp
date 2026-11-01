#include "ExcalCore/EngineInitializer.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

EngineInitializer* EngineInitializer::instance = nullptr;

EngineInitializer::EngineInitializer(ApplicationSpec& spec) {
  Debug::InitLogging("log.txt");
  Debug::Log("Initialized Logging System...");

  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Attempted to initialize a second instance of EngineInitializer."
    );
  }

  instance = this;

  Debug::Log("Initializing SDL...");

  if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    Debug::ThrowError(
      Debug::LogCategory::CATEGORY_ENGINE,
      "Failed to initialize SDL: {}",
      SDL_GetError()
    );
  }

  Debug::Log("Initializing TTF Engine...");

  if (!TTF_Init()) {
    Debug::ThrowError(
      Debug::LogCategory::CATEGORY_ENGINE,
      "Failed to initialize TTF Engine: {}",
      SDL_GetError()
    );
  }
}

EngineInitializer::~EngineInitializer() {
  Debug::Log("Destroying TTF Engine...");
  TTF_Quit();

  Debug::Log("Quitting SDL...");
  SDL_Quit();

  instance = nullptr;
  Debug::ShutdownLogging();
}

EngineInitializer* EngineInitializer::GetInstance() {
  if (!instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE, "EngineInitializer has not been initialized."
    );
  }

  return instance;
}

}
