#pragma once

#include <concepts>
#include <memory>
#include <string>

namespace ExcalCore {

template <typename T>
concept LoadableAsset = requires(const std::string& path) {
  { T::Load(path) } -> std::same_as<std::shared_ptr<T>>;
};

struct Asset { };

template <typename T>
concept AssetType = std::derived_from<T, Asset>;

}
