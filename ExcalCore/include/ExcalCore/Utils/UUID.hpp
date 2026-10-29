#pragma once

#include <cstdint>

namespace ExcalCore {

struct UUID {
  uint64_t value;
};

class UUIDManager final {
public:
  static UUID GetNewUUID();
};

}
