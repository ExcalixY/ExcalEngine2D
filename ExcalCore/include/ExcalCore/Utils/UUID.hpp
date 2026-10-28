#pragma once

#include <cstdint>
#include <random>

namespace ExcalCore {

struct UUID {
  uint64_t value;
};

class UUIDManager final {
public:
  static UUID GetNewUUID() {
    static std::random_device device;
    static std::mt19937_64 engine(device());
    static std::uniform_int_distribution<uint64_t> generator;

    return {generator(engine)};
  }
};

}
