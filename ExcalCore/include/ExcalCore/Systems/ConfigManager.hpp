#pragma once

#include <string>

#include "ExcalCore/ApplicationSpec.hpp"

namespace ExcalCore {

class ConfigManager final {
public:
  explicit ConfigManager(ApplicationSpec& spec);

  template <typename T>
  T Load(const std::string& path, const T& defaults);

  template <typename T>
  T Write(const T& object, const std::string& path);

private:
};

}
