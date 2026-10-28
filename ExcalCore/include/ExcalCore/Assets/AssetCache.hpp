#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <system_error>
#include <unordered_map>

#include "Asset.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

template <typename T>
  requires LoadableAsset<T>
class AssetCache {
public:
  explicit AssetCache() : m_Data(), m_PinnedData() { }

  ~AssetCache() {
    m_Data.clear();
    m_PinnedData.clear();
  }

  std::shared_ptr<T> LoadAsset(const std::string& path) {
    std::error_code ec;
    std::filesystem::path real_path = std::filesystem::canonical(path, ec);

    if (ec) {
      Debug::LogError(
        Debug::CATEGORY_ASSETS,
        "Asset does not exist \"{}\": {}",
        path,
        ec.message()
      );

      return nullptr;
    }

    std::string path_str = real_path.generic_string();

    if (auto it = m_Data.find(path_str); it != m_Data.end()) {
      if (auto asset = it->second.lock()) {
        return asset;
      }
    }

    std::shared_ptr<T> asset = T::Load(path);
    if (!asset) {
      return nullptr;
    }

    std::weak_ptr<T> ptr_weak = std::weak_ptr<T>(asset);
    m_Data.insert_or_assign(path_str, ptr_weak);

    return asset;
  }

  void Sweep() {
    std::erase_if(m_Data, [](const auto& entry) {
      return entry.second.expired();
    });
  }

private:
  std::unordered_map<std::string, std::weak_ptr<T>> m_Data;
  std::unordered_map<std::string, std::shared_ptr<T>> m_PinnedData;
};

}
