#pragma once

#include <memory>
#include <string>

#include "Asset.hpp"

namespace ExcalCore {

class FontAsset : public Asset {
public:
  static std::shared_ptr<FontAsset> Load(const std::string& path);

private:
};

}
