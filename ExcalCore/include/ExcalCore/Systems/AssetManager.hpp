#pragma once

#include <memory>
#include <tuple>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Assets/AssetCache.hpp"
#include "ExcalCore/Assets/AudioClip.hpp"
#include "ExcalCore/Assets/FontAsset.hpp"
#include "ExcalCore/Assets/TextureAsset.hpp"

namespace ExcalCore {

class AssetManager final {
public:
  explicit AssetManager(ApplicationSpec& spec)
      : m_AssetDir(spec.assetSpec.directory) { }

  template <typename T, typename... Args>
  std::shared_ptr<T> LoadAsset(const std::string& path, Args&&... args) {
    return std::get<AssetCache<T>>(m_Caches).LoadAsset(
      path, std::forward<Args>(args)...
    );
  }

private:
  std::tuple<
    AssetCache<AudioClip>,
    AssetCache<TextureAsset>,
    AssetCache<FontAsset>>
    m_Caches;

  std::string m_AssetDir;
};

}
