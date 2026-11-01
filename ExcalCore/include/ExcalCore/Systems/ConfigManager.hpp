#pragma once

#include <string>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

class ConfigManager final {
public:
  explicit ConfigManager(ApplicationSpec& spec) {
    if (instance) {
      Debug::ThrowError(
        Debug::CATEGORY_ENGINE,
        "Attempted to initialize a second instance of ConfigManager."
      );
    }

    Debug::Log("Initializing Config Manager..");
    instance = this;
  }

  ~ConfigManager() {
    instance = nullptr;
  }

  template <typename T>
  [[nodiscard]]
  T Load(const std::string& path, const T& defaults);

  template <typename T>
  T Write(const T& object, const std::string& path);

private:
  static ConfigManager* instance;
};

}
