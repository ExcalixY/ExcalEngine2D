#pragma once

#include <string>

#include "ExcalCore/ApplicationSpec.hpp"

namespace ExcalCore {

class ConfigManager final {
public:
  ConfigManager(const ConfigManager&) = default;
  ConfigManager(ConfigManager&&) = delete;
  ConfigManager& operator=(const ConfigManager&) = default;
  ConfigManager& operator=(ConfigManager&&) = delete;

  explicit ConfigManager(ApplicationSpec& spec);
  ~ConfigManager();

  template <typename T>
  [[nodiscard]]
  T Load(const std::string& path, const T& defaults);

  template <typename T>
  T Write(const T& object, const std::string& path);

private:
  static ConfigManager* instance;
};

}
