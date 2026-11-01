#include "ExcalCore/Systems/AssetManager.hpp"

#include <SDL3/SDL_filesystem.h>

#include <string>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

AssetManager* AssetManager::instance = nullptr;

AssetManager::AssetManager(ApplicationSpec& spec)
    : m_AssetDir(
        std::string(SDL_GetBasePath()) + spec.assetSpec.directory + "/"
      ) {
  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Attempted to initialize a second instance of AssetManager."
    );
  }

  instance = this;
  Debug::Log("Initializing Asset Manager...");
}

AssetManager::~AssetManager() {
  instance = nullptr;
  Debug::Log("Destroying Asset Manager...");
}

AssetManager* AssetManager::GetInstance() {
  if (!instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ASSETS, "Asset Manager has not been initialized."
    );
  }

  return instance;
}

}
