#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ExcalCore/ECS/Components/Component.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"

using namespace ExcalCore;

namespace {

struct Position : Component {
  Position() = default;
  Position(float x, float y) : x(x), y(y) { }

  float x = 0.f;
  float y = 0.f;
};

struct Velocity : Component {
  Velocity() = default;
  Velocity(float dx, float dy) : dx(dx), dy(dy) { }

  float dx = 0.f;
  float dy = 0.f;
};

struct Health : Component {
  Health() = default;
  explicit Health(int hp) : hp(hp) { }

  int hp = 100;
};

struct Name : Component {
  Name() = default;
  explicit Name(std::string value) : value(std::move(value)) { }

  std::string value;
};

struct Tag : Component { };

bool SameEntity(Entity a, Entity b) {
  return a.id == b.id && a.gen == b.gen;
}

void MovePosition(Entity, Position* position, Velocity* velocity) {
  position->x += velocity->dx;
  position->y += velocity->dy;
}

}

TEST_CASE("ECSRegistry::NewEntity", "[ecs]") {
  ECSRegistry ecs;

  SECTION("hands out sequential ids starting at 0 with generation 0") {
    for (uint32_t i = 0; i < 10; ++i) {
      Entity e = ecs.NewEntity();
      REQUIRE(e.id == i);
      REQUIRE(e.gen == 0);
    }
  }

  SECTION("every live entity has a unique id") {
    std::unordered_set<uint32_t> ids;
    for (int i = 0; i < 1000; ++i) {
      REQUIRE(ids.insert(ecs.NewEntity().id).second);
    }
  }
}

TEST_CASE("ECSRegistry::DeleteEntity recycles ids", "[ecs]") {
  ECSRegistry ecs;

  SECTION("a deleted id comes back with a higher generation") {
    Entity a = ecs.NewEntity();
    (void)ecs.NewEntity();
    ecs.DeleteEntity(a);

    Entity reused = ecs.NewEntity();
    REQUIRE(reused.id == a.id);
    REQUIRE(reused.gen > a.gen);
  }

  SECTION("an id can be recycled several times") {
    Entity e = ecs.NewEntity();
    for (uint32_t round = 1; round <= 5; ++round) {
      ecs.DeleteEntity(e);
      Entity next = ecs.NewEntity();
      REQUIRE(next.id == e.id);
      REQUIRE(next.gen > e.gen);
      e = next;
    }
  }

  SECTION("fresh ids continue after the recycled ones run out") {
    Entity a = ecs.NewEntity();
    Entity b = ecs.NewEntity();
    Entity c = ecs.NewEntity();
    ecs.DeleteEntity(a);
    ecs.DeleteEntity(b);

    std::unordered_set<uint32_t> ids;
    ids.insert(ecs.NewEntity().id);
    ids.insert(ecs.NewEntity().id);
    REQUIRE(ids == std::unordered_set<uint32_t>{a.id, b.id});

    Entity fresh = ecs.NewEntity();
    REQUIRE(fresh.id == c.id + 1);
    REQUIRE(fresh.gen == 0);
  }
}

TEST_CASE("ECSRegistry::AddComponent", "[ecs]") {
  ECSRegistry ecs;
  Entity e = ecs.NewEntity();

  SECTION("returns a pointer to the constructed component") {
    Position* position = ecs.AddComponent<Position>(e, 3.f, 4.f);

    REQUIRE(position != nullptr);
    REQUIRE(position->x == 3.f);
    REQUIRE(position->y == 4.f);
  }

  SECTION("default-constructs when no arguments are given") {
    Health* health = ecs.AddComponent<Health>(e);

    REQUIRE(health != nullptr);
    REQUIRE(health->hp == 100);
  }

  SECTION("works with non-trivial and empty components") {
    Name* name = ecs.AddComponent<Name>(e, "Player");
    Tag* tag = ecs.AddComponent<Tag>(e);

    REQUIRE(name != nullptr);
    REQUIRE(name->value == "Player");
    REQUIRE(tag != nullptr);
  }

  SECTION("adding the same type twice is rejected and keeps the first") {
    Position* first = ecs.AddComponent<Position>(e, 1.f, 1.f);
    Position* second = ecs.AddComponent<Position>(e, 2.f, 2.f);

    REQUIRE(first != nullptr);
    REQUIRE(second == nullptr);
    REQUIRE(ecs.GetComponent<Position>(e)->x == 1.f);
  }

  SECTION("one entity can hold several component types") {
    ecs.AddComponent<Position>(e, 1.f, 2.f);
    ecs.AddComponent<Velocity>(e, 3.f, 4.f);
    ecs.AddComponent<Health>(e, 50);

    REQUIRE(ecs.GetComponent<Position>(e)->x == 1.f);
    REQUIRE(ecs.GetComponent<Velocity>(e)->dx == 3.f);
    REQUIRE(ecs.GetComponent<Health>(e)->hp == 50);
  }
}

TEST_CASE("ECSRegistry::GetComponent", "[ecs]") {
  ECSRegistry ecs;
  Entity a = ecs.NewEntity();
  Entity b = ecs.NewEntity();

  SECTION("returns the same component AddComponent created") {
    Position* added = ecs.AddComponent<Position>(a, 5.f, 6.f);

    REQUIRE(ecs.GetComponent<Position>(a) == added);
  }

  SECTION("writes through the pointer persist") {
    ecs.AddComponent<Position>(a, 0.f, 0.f);
    ecs.GetComponent<Position>(a)->x = 42.f;

    REQUIRE(ecs.GetComponent<Position>(a)->x == 42.f);
  }

  SECTION("returns nullptr when the entity lacks the component") {
    ecs.AddComponent<Position>(a, 1.f, 1.f);

    REQUIRE(ecs.GetComponent<Position>(b) == nullptr);
    REQUIRE(ecs.GetComponent<Velocity>(a) == nullptr);
  }

  SECTION("returns nullptr for a type that has never been added") {
    REQUIRE(ecs.GetComponent<Health>(a) == nullptr);
  }

  SECTION("component types are stored independently") {
    ecs.AddComponent<Position>(a, 1.f, 2.f);
    ecs.AddComponent<Velocity>(b, 3.f, 4.f);

    REQUIRE(ecs.GetComponent<Position>(a)->x == 1.f);
    REQUIRE(ecs.GetComponent<Velocity>(b)->dx == 3.f);
    REQUIRE(ecs.GetComponent<Velocity>(a) == nullptr);
    REQUIRE(ecs.GetComponent<Position>(b) == nullptr);
  }

  SECTION("many entities keep their own data") {
    std::vector<Entity> entities;
    for (int i = 0; i < 1000; ++i) {
      Entity e = ecs.NewEntity();
      entities.push_back(e);
      ecs.AddComponent<Health>(e, i);
    }

    for (int i = 0; i < 1000; ++i) {
      REQUIRE(ecs.GetComponent<Health>(entities[i])->hp == i);
    }
  }
}

TEST_CASE("ECSRegistry::Each with one component type", "[ecs][each]") {
  ECSRegistry ecs;
  std::vector<Entity> entities;
  for (int i = 0; i < 5; ++i) {
    Entity e = ecs.NewEntity();
    entities.push_back(e);
    ecs.AddComponent<Health>(e, i * 10);
  }
  ecs.AddComponent<Position>(ecs.NewEntity(), 0.f, 0.f);

  SECTION("visits every entity with the component exactly once") {
    std::unordered_set<uint32_t> visited;
    ecs.Each<Health>([&](Entity e, Health* health) {
      REQUIRE(health != nullptr);
      REQUIRE(visited.insert(e.id).second);
    });

    REQUIRE(visited.size() == 5);
    for (Entity e : entities) {
      REQUIRE(visited.contains(e.id));
    }
  }

  SECTION("passes the component that belongs to the entity") {
    ecs.Each<Health>([&](Entity e, Health* health) {
      REQUIRE(health == ecs.GetComponent<Health>(e));
    });
  }

  SECTION("writes through the component pointer persist") {
    ecs.Each<Health>([](Entity, Health* health) { health->hp += 1; });

    for (int i = 0; i < 5; ++i) {
      REQUIRE(ecs.GetComponent<Health>(entities[i])->hp == i * 10 + 1);
    }
  }

  SECTION("a type without any entities is visited zero times") {
    int calls = 0;
    ecs.Each<Velocity>([&](Entity, Velocity*) { ++calls; });

    REQUIRE(calls == 0);
  }
}

TEST_CASE("ECSRegistry::Each with several component types", "[ecs][each]") {
  ECSRegistry ecs;

  Entity position_only = ecs.NewEntity();
  Entity velocity_only = ecs.NewEntity();
  Entity both = ecs.NewEntity();
  Entity all_three = ecs.NewEntity();
  Entity position_only_2 = ecs.NewEntity();
  Entity nothing = ecs.NewEntity();

  ecs.AddComponent<Position>(position_only, 1.f, 1.f);
  ecs.AddComponent<Position>(position_only_2, 2.f, 2.f);
  ecs.AddComponent<Position>(both, 10.f, 20.f);
  ecs.AddComponent<Position>(all_three, 30.f, 40.f);

  ecs.AddComponent<Velocity>(velocity_only, 9.f, 9.f);
  ecs.AddComponent<Velocity>(both, 1.f, 2.f);
  ecs.AddComponent<Velocity>(all_three, 3.f, 4.f);

  ecs.AddComponent<Health>(all_three, 7);

  (void)nothing;

  SECTION("visits exactly the entities that have every requested type") {
    std::unordered_set<uint32_t> visited;
    ecs.Each<Position, Velocity>([&](Entity e, Position*, Velocity*) {
      visited.insert(e.id);
    });

    REQUIRE(visited == std::unordered_set<uint32_t>{both.id, all_three.id});
  }

  SECTION("never passes a null component") {
    ecs.Each<Position, Velocity>([](Entity, Position* p, Velocity* v) {
      REQUIRE(p != nullptr);
      REQUIRE(v != nullptr);
    });
  }

  SECTION("passes the components that belong to the entity") {
    ecs.Each<Position, Velocity>([&](Entity e, Position* p, Velocity* v) {
      REQUIRE(p == ecs.GetComponent<Position>(e));
      REQUIRE(v == ecs.GetComponent<Velocity>(e));
    });
  }

  SECTION("the order of the template arguments doesn't change the result") {
    std::unordered_set<uint32_t> forward;
    std::unordered_set<uint32_t> reversed;
    ecs.Each<Position, Velocity>([&](Entity e, Position*, Velocity*) {
      forward.insert(e.id);
    });
    ecs.Each<Velocity, Position>([&](Entity e, Velocity*, Position*) {
      reversed.insert(e.id);
    });

    REQUIRE(forward == reversed);
  }

  SECTION("three types visit only the entity with all three") {
    std::vector<Entity> visited;
    ecs.Each<Position, Velocity, Health>(
      [&](Entity e, Position*, Velocity*, Health* health) {
        visited.push_back(e);
        REQUIRE(health->hp == 7);
      }
    );

    REQUIRE(visited.size() == 1);
    REQUIRE(SameEntity(visited[0], all_three));
  }

  SECTION("an empty pool among the requested types means zero visits") {
    int calls = 0;
    ecs.Each<Position, Tag>([&](Entity, Position*, Tag*) { ++calls; });

    REQUIRE(calls == 0);
  }

  SECTION("a free function works as the callback and can modify components") {
    ecs.Each<Position, Velocity>(MovePosition);

    REQUIRE(ecs.GetComponent<Position>(both)->x == 11.f);
    REQUIRE(ecs.GetComponent<Position>(both)->y == 22.f);
    REQUIRE(ecs.GetComponent<Position>(all_three)->x == 33.f);
    REQUIRE(ecs.GetComponent<Position>(position_only)->x == 1.f);
  }
}

TEST_CASE("ECSRegistry deleted entities", "[ecs]") {
  ECSRegistry ecs;
  Entity a = ecs.NewEntity();
  Entity b = ecs.NewEntity();
  ecs.AddComponent<Position>(a, 1.f, 1.f);
  ecs.AddComponent<Position>(b, 2.f, 2.f);

  SECTION("a deleted entity's components are no longer reachable") {
    ecs.DeleteEntity(a);

    REQUIRE(ecs.GetComponent<Position>(a) == nullptr);
  }

  SECTION("Each skips deleted entities") {
    ecs.DeleteEntity(a);

    std::vector<Entity> visited;
    ecs.Each<Position>([&](Entity e, Position*) { visited.push_back(e); });

    REQUIRE(visited.size() == 1);
    REQUIRE(SameEntity(visited[0], b));
  }

  SECTION("a recycled id starts without the old components") {
    ecs.DeleteEntity(a);
    Entity reused = ecs.NewEntity();

    REQUIRE(reused.id == a.id);
    REQUIRE(ecs.GetComponent<Position>(reused) == nullptr);
    REQUIRE(ecs.GetComponent<Position>(a) == nullptr);
    REQUIRE(ecs.AddComponent<Position>(reused, 5.f, 5.f) != nullptr);
  }

  SECTION("deleting the same entity twice doesn't duplicate its id") {
    ecs.DeleteEntity(a);
    ecs.DeleteEntity(a);

    Entity first = ecs.NewEntity();
    Entity second = ecs.NewEntity();
    REQUIRE(first.id != second.id);
  }
}

TEST_CASE("ESCRegistry is alive", "[ecs][isalive]") {
  ECSRegistry ecs;
  Entity a = ecs.NewEntity();

  SECTION("Check if entity is alive") {
    REQUIRE(ecs.IsAlive(a) == true);
  }
}
