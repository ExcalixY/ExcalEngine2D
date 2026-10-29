#include "ExcalCore/Systems/EventSystem.hpp"

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

EventSystem::EventSystem(ApplicationSpec& spec) {
  Debug::Log("Initializing Event System...");
}

EventSystem::~EventSystem() {
  Debug::Log("Destroying Event System...");
}

}
