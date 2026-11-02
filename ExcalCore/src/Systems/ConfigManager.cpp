#include "ExcalCore/Systems/ConfigManager.hpp"

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

ConfigManager::ConfigManager(ApplicationSpec& spec) {
  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Attempted to initialize a second instance of ConfigManager."
    );
  }

  Debug::Log("Initializing Config Manager..");
  instance = this;
}

ConfigManager::~ConfigManager() {
  instance = nullptr;
}

}
