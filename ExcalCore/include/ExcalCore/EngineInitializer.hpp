#pragma once

#include "ApplicationSpec.hpp"

namespace ExcalCore {

struct EngineInitializer {
  explicit EngineInitializer(ApplicationSpec& spec);
  ~EngineInitializer();

  EngineInitializer(const EngineInitializer&) = delete;
  EngineInitializer& operator=(const EngineInitializer&) = delete;

  [[nodiscard]]
  static EngineInitializer* GetInstance();

private:
  static EngineInitializer* instance;
};

}
