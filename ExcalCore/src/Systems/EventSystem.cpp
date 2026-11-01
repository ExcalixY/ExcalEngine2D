#include "ExcalCore/Systems/EventSystem.hpp"

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

EventSystem* EventSystem::instance = nullptr;

EventSystem::EventSystem(ApplicationSpec& spec) {
  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Attempted to initialize a second instance of EventSystem."
    );
  }

  instance = this;
  Debug::Log("Initializing Event System...");
}

EventSystem::~EventSystem() {
  instance = nullptr;
  Debug::Log("Destroying Event System...");
}

EventSystem* EventSystem::GetInstance() {
  if (!instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE, "Event System has not been initialized."
    );
  }

  return instance;
}

}
