#pragma once

#include <cstdint>
#include <limits>

namespace ExcalCore {

struct Entity {
  uint32_t id;
  uint32_t gen;
};

static constexpr Entity EMPTY_ENTITY = {
  .id = std::numeric_limits<uint32_t>::max(),
  .gen = std::numeric_limits<uint32_t>::max()
};

}
