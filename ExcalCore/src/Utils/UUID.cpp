#include "ExcalCore/Utils/UUID.hpp"

#include <random>

namespace ExcalCore {

UUID UUIDManager::GetNewUUID() {
  static std::random_device device;
  static std::mt19937_64 engine(device());
  static std::uniform_int_distribution<uint64_t> generator;

  return {generator(engine)};
}

}
