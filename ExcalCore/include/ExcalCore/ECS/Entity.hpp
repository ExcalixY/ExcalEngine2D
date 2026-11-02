#pragma once

#include <cstdint>
#include <format>
#include <limits>

namespace ExcalCore {

struct Entity {
  uint32_t id;
  uint32_t gen;

  bool operator==(const Entity& other) const {
    return (other.id == id && other.gen == gen);
  }
};

static constexpr Entity EMPTY_ENTITY = {
  std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max()
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
