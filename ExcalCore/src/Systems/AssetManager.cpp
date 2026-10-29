#include "ExcalCore/Systems/AssetManager.hpp"

namespace ExcalCore {

AssetManager::AssetManager(ApplicationSpec& spec)
    : m_AssetDir(spec.assetSpec.directory) { }

}
