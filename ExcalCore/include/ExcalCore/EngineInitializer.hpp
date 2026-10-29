#pragma once

#include "ApplicationSpec.hpp"

namespace ExcalCore {

struct EngineInitializer {
  explicit EngineInitializer(ApplicationSpec& spec);
  ~EngineInitializer();
};

}
