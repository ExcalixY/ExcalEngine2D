#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Assets/Asset.hpp"
#include "ExcalCore/Assets/AssetCache.hpp"

namespace ExcalCore {

class AssetManager final {
public:
  explicit AssetManager(ApplicationSpec& spec);
  ~AssetManager();

  AssetManager(const AssetManager&) = delete;
  AssetManager& operator=(const AssetManager&) = delete;

  [[nodiscard]]
  static AssetManager* GetInstance();

  template <LoadableAsset T, typename... Args>
  [[nodiscard]]
  std::shared_ptr<T> LoadAsset(const std::string& path, Args&&... args);

private:
  static AssetManager* instance;

  template <LoadableAsset T>
  AssetCache<T>& GetCache();

  std::unordered_map<std::type_index, std::unique_ptr<IAssetCache>> m_Caches;

  std::string m_AssetDir;
};

template <LoadableAsset T, typename... Args>
std::shared_ptr<T> AssetManager::LoadAsset(
  const std::string& path, Args&&... args
) {
  return GetCache<T>().LoadAsset(
    m_AssetDir + path, std::forward<Args>(args)...
  );
}

template <LoadableAsset T>
AssetCache<T>& AssetManager::GetCache() {
  auto [it, inserted] =
    m_Caches.try_emplace(std::type_index(typeid(T)), nullptr);

  if (inserted) {
    it->second = std::make_unique<AssetCache<T>>();
  }

  return static_cast<AssetCache<T>&>(*it->second);
}

namespace Assets {

template <LoadableAsset T, typename... Args>
[[nodiscard]]
std::shared_ptr<T> LoadAsset(const std::string& path, Args&&... args) {
  return AssetManager::GetInstance()->LoadAsset<T>(
    path, std::forward<Args>(args)...
  );
}

}

}
