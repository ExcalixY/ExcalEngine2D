#pragma once

#include <cstdint>
#include <format>
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

template <>
struct std::formatter<ExcalCore::Entity> {
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  auto format(const ExcalCore::Entity& e, std::format_context& ctx) const {
    return std::format_to(ctx.out(), "[id: {}, gen: {}]", e.id, e.gen);
  }
};
