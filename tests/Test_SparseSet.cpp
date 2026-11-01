#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ExcalCore/ECS/Entity.hpp"
#include "ExcalCore/Utils/SparseSet.hpp"
#include <catch2/catch_message.hpp>

using namespace ExcalCore;

namespace {

bool SameEntity(Entity a, Entity b) {
  return a.id == b.id && a.gen == b.gen;
}

struct Vec2i {
  Vec2i(int x, int y) : x(x), y(y) { }

  int x;
  int y;
};

struct Tracked {
  static inline int alive = 0;

  explicit Tracked(int value) : value(value) {
    ++alive;
  }
  Tracked(const Tracked& other) : value(other.value) {
    ++alive;
  }
  Tracked(Tracked&& other) noexcept : value(other.value) {
    ++alive;
  }
  Tracked& operator=(const Tracked&) = default;
  Tracked& operator=(Tracked&&) noexcept = default;
  ~Tracked() {
    --alive;
  }

  int value;
};

}

TEST_CASE("SparseSet starts empty", "[sparseset]") {
  SparseSet<int> set;

  REQUIRE(set.Size() == 0);
  REQUIRE(set.GetEntities().empty());
  REQUIRE(set.GetData().empty());

  REQUIRE_FALSE(set.Contains({0, 0}));
  REQUIRE_FALSE(set.Contains({7, 0}));
  REQUIRE_FALSE(set.Contains({8, 0}));
  REQUIRE_FALSE(set.Contains({100000, 0}));
  REQUIRE_FALSE(set.Contains(EMPTY_ENTITY));

  REQUIRE(set.TryGet({0, 0}) == nullptr);
  REQUIRE(set.TryGet(EMPTY_ENTITY) == nullptr);

  REQUIRE_THROWS_AS(set.Get({0, 0}), std::runtime_error);
  REQUIRE_THROWS_AS(set.Get(EMPTY_ENTITY), std::runtime_error);
}

TEST_CASE("SparseSet::Emplace", "[sparseset]") {
  SparseSet<int> set;

  SECTION("a single entity") {
    Entity e{0, 0};
    int* value = set.Emplace(e, 42);

    REQUIRE(value != nullptr);
    REQUIRE(*value == 42);
    REQUIRE(value == &set.Get(e));
    REQUIRE(value == set.TryGet(e));
    REQUIRE(value == &set.GetData()[0]);

    REQUIRE(set.Size() == 1);
    REQUIRE(set.Contains(e));
    REQUIRE(set.Get(e) == 42);
    REQUIRE(set.GetEntities().size() == 1);
    REQUIRE(SameEntity(set.GetEntities()[0], e));
    REQUIRE(set.GetData()[0] == 42);
  }

  SECTION("several entities keep insertion order in the dense arrays") {
    Entity a{3, 0};
    Entity b{1, 0};
    Entity c{5, 2};
    set.Emplace(a, 10);
    set.Emplace(b, 20);
    set.Emplace(c, 30);

    REQUIRE(set.Size() == 3);

    auto entities = set.GetEntities();
    auto data = set.GetData();
    REQUIRE(SameEntity(entities[0], a));
    REQUIRE(SameEntity(entities[1], b));
    REQUIRE(SameEntity(entities[2], c));
    REQUIRE(data[0] == 10);
    REQUIRE(data[1] == 20);
    REQUIRE(data[2] == 30);

    REQUIRE(set.Get(a) == 10);
    REQUIRE(set.Get(b) == 20);
    REQUIRE(set.Get(c) == 30);
  }

  SECTION("writes through the returned pointer are visible") {
    Entity e{6, 1};
    int* value = set.Emplace(e, 1);
    *value = 77;

    REQUIRE(set.Get(e) == 77);
    REQUIRE(set.GetData()[0] == 77);
  }

  SECTION("the returned pointer refers to the newest element") {
    Entity a{0, 0};
    Entity b{1, 0};
    set.Emplace(a, 10);
    int* value = set.Emplace(b, 20);

    REQUIRE(value != nullptr);
    REQUIRE(*value == 20);
    REQUIRE(value == &set.Get(b));
    REQUIRE(value == &set.GetData()[1]);
  }

  SECTION("ids around and past the starting sparse size") {
    std::vector<Entity> entities = {
      {START_SPARSE_SIZE - 1, 0},
      {START_SPARSE_SIZE, 0},
      {START_SPARSE_SIZE + 1, 0},
      {START_SPARSE_SIZE * 2, 0},
      {1000, 0},
      {100000, 0},
    };

    for (size_t i = 0; i < entities.size(); ++i) {
      INFO("id = " << entities[i].id);
      int* value = set.Emplace(entities[i], static_cast<int>(i));
      REQUIRE(value != nullptr);
      REQUIRE(*value == static_cast<int>(i));
      REQUIRE(value == set.TryGet(entities[i]));
    }

    REQUIRE(set.Size() == entities.size());
    for (size_t i = 0; i < entities.size(); ++i) {
      INFO("id = " << entities[i].id);
      REQUIRE(set.Contains(entities[i]));
      REQUIRE(set.Get(entities[i]) == static_cast<int>(i));
    }
  }

  SECTION("a large id first, then small ids") {
    Entity big{5000, 0};
    Entity small{0, 0};
    set.Emplace(big, 1);
    set.Emplace(small, 2);

    REQUIRE(set.Get(big) == 1);
    REQUIRE(set.Get(small) == 2);
    REQUIRE_FALSE(set.Contains({4999, 0}));
    REQUIRE_FALSE(set.Contains({5001, 0}));
  }

  SECTION("the same entity twice is ignored") {
    Entity e{2, 0};
    int* first = set.Emplace(e, 1);
    int* second = set.Emplace(e, 2);

    REQUIRE(first != nullptr);
    REQUIRE(second == nullptr);
    REQUIRE(first == &set.Get(e));
    REQUIRE(set.Size() == 1);
    REQUIRE(set.Get(e) == 1);
    REQUIRE(set.GetEntities().size() == 1);
    REQUIRE(set.GetData().size() == 1);
  }

  SECTION("entities with non-zero generations") {
    Entity e{4, 17};
    set.Emplace(e, 99);

    REQUIRE(set.Contains(e));
    REQUIRE_FALSE(set.Contains({4, 0}));
    REQUIRE_FALSE(set.Contains({4, 16}));
    REQUIRE_FALSE(set.Contains({4, 18}));
    REQUIRE(set.Get(e) == 99);
  }
}

TEST_CASE("SparseSet::Emplace forwards constructor arguments", "[sparseset]") {
  SECTION("multi-argument constructor") {
    SparseSet<Vec2i> set;
    Entity e{1, 0};
    Vec2i* value = set.Emplace(e, 3, -4);

    REQUIRE(value != nullptr);
    REQUIRE(value->x == 3);
    REQUIRE(value->y == -4);
    REQUIRE(value == &set.Get(e));
    REQUIRE(set.Get(e).x == 3);
    REQUIRE(set.Get(e).y == -4);
  }

  SECTION("std::string") {
    SparseSet<std::string> set;
    Entity a{0, 0};
    Entity b{1, 0};
    std::string* hello = set.Emplace(a, "hello");
    std::string* xs = set.Emplace(b, 5, 'x');

    REQUIRE(hello != nullptr);
    REQUIRE(xs != nullptr);
    REQUIRE(*xs == "xxxxx");
    REQUIRE(xs == &set.Get(b));
    REQUIRE(set.Get(a) == "hello");
    REQUIRE(set.Get(b) == "xxxxx");
  }

  SECTION("move-only type") {
    SparseSet<std::unique_ptr<int>> set;
    Entity a{0, 0};
    Entity b{1, 0};
    Entity c{2, 0};
    set.Emplace(a, std::make_unique<int>(1));
    set.Emplace(b, std::make_unique<int>(2));
    std::unique_ptr<int>* last = set.Emplace(c, std::make_unique<int>(3));

    REQUIRE(last != nullptr);
    REQUIRE(*last != nullptr);
    REQUIRE(**last == 3);
    REQUIRE(last == &set.Get(c));

    set.Remove(a);

    REQUIRE(set.Size() == 2);
    REQUIRE_FALSE(set.Contains(a));
    REQUIRE(*set.Get(b) == 2);
    REQUIRE(*set.Get(c) == 3);
  }

  SECTION("no arguments value-initializes") {
    SparseSet<int> set;
    Entity e{0, 0};
    int* value = set.Emplace(e);

    REQUIRE(value != nullptr);
    REQUIRE(*value == 0);
    REQUIRE(set.Get(e) == 0);
  }
}

TEST_CASE("SparseSet::Contains", "[sparseset]") {
  SparseSet<int> set;
  Entity a{0, 0};
  Entity b{1, 3};
  Entity c{20, 1};
  set.Emplace(a, 1);
  set.Emplace(b, 2);
  set.Emplace(c, 3);

  SECTION("finds every inserted entity") {
    REQUIRE(set.Contains(a));
    REQUIRE(set.Contains(b));
    REQUIRE(set.Contains(c));
  }

  SECTION("rejects a different generation of the same id") {
    REQUIRE_FALSE(set.Contains({0, 1}));
    REQUIRE_FALSE(set.Contains({1, 2}));
    REQUIRE_FALSE(set.Contains({1, 4}));
    REQUIRE_FALSE(set.Contains({20, 0}));
  }

  SECTION("rejects ids that were never inserted") {
    REQUIRE_FALSE(set.Contains({2, 0}));
    REQUIRE_FALSE(set.Contains({19, 1}));
    REQUIRE_FALSE(set.Contains({21, 1}));
    REQUIRE_FALSE(set.Contains({1000000, 0}));
    REQUIRE_FALSE(set.Contains(EMPTY_ENTITY));
  }

  SECTION("is false after removal and does not affect others") {
    set.Remove(b);

    REQUIRE_FALSE(set.Contains(b));
    REQUIRE(set.Contains(a));
    REQUIRE(set.Contains(c));
  }

  SECTION("works through a const reference") {
    const SparseSet<int>& const_set = set;

    REQUIRE(const_set.Contains(a));
    REQUIRE_FALSE(const_set.Contains({2, 0}));
  }
}

TEST_CASE("SparseSet::Remove", "[sparseset]") {
  SparseSet<int> set;
  Entity a{0, 0};
  Entity b{1, 0};
  Entity c{2, 0};
  Entity d{3, 0};

  SECTION("the only element") {
    set.Emplace(a, 1);
    set.Remove(a);

    REQUIRE(set.Size() == 0);
    REQUIRE_FALSE(set.Contains(a));
    REQUIRE(set.TryGet(a) == nullptr);
    REQUIRE(set.GetEntities().empty());
    REQUIRE(set.GetData().empty());
  }

  SECTION("the last dense element") {
    set.Emplace(a, 1);
    set.Emplace(b, 2);
    set.Emplace(c, 3);
    set.Remove(c);

    REQUIRE(set.Size() == 2);
    REQUIRE_FALSE(set.Contains(c));
    REQUIRE(set.Get(a) == 1);
    REQUIRE(set.Get(b) == 2);
    REQUIRE(SameEntity(set.GetEntities()[0], a));
    REQUIRE(SameEntity(set.GetEntities()[1], b));
  }

  SECTION("the first dense element moves the last one into its slot") {
    set.Emplace(a, 1);
    set.Emplace(b, 2);
    set.Emplace(c, 3);
    set.Emplace(d, 4);
    set.Remove(a);

    REQUIRE(set.Size() == 3);
    REQUIRE_FALSE(set.Contains(a));

    REQUIRE(SameEntity(set.GetEntities()[0], d));
    REQUIRE(set.GetData()[0] == 4);

    REQUIRE(set.Contains(d));
    REQUIRE(set.Get(d) == 4);
    REQUIRE(set.Get(b) == 2);
    REQUIRE(set.Get(c) == 3);
  }

  SECTION("a middle element") {
    set.Emplace(a, 1);
    set.Emplace(b, 2);
    set.Emplace(c, 3);
    set.Emplace(d, 4);
    set.Remove(b);

    REQUIRE(set.Size() == 3);
    REQUIRE_FALSE(set.Contains(b));
    REQUIRE(SameEntity(set.GetEntities()[1], d));
    REQUIRE(set.GetData()[1] == 4);
    REQUIRE(set.Get(a) == 1);
    REQUIRE(set.Get(c) == 3);
    REQUIRE(set.Get(d) == 4);
  }

  SECTION("several removals in a row") {
    set.Emplace(a, 1);
    set.Emplace(b, 2);
    set.Emplace(c, 3);
    set.Emplace(d, 4);

    set.Remove(a);
    set.Remove(d);

    REQUIRE(set.Size() == 2);
    REQUIRE_FALSE(set.Contains(a));
    REQUIRE_FALSE(set.Contains(d));
    REQUIRE(set.Get(b) == 2);
    REQUIRE(set.Get(c) == 3);

    set.Remove(c);
    set.Remove(b);

    REQUIRE(set.Size() == 0);
    REQUIRE_FALSE(set.Contains(b));
    REQUIRE_FALSE(set.Contains(c));
  }

  SECTION("every removal order of four entities") {
    std::vector<int> order = {0, 1, 2, 3};
    std::vector<Entity> entities = {a, b, c, d};

    do {
      SparseSet<int> local;
      for (int i = 0; i < 4; ++i) {
        local.Emplace(entities[i], i * 10);
      }

      for (size_t step = 0; step < order.size(); ++step) {
        local.Remove(entities[order[step]]);

        REQUIRE(local.Size() == 3 - step);
        for (size_t k = 0; k < order.size(); ++k) {
          Entity e = entities[order[k]];
          if (k <= step) {
            REQUIRE_FALSE(local.Contains(e));
          } else {
            REQUIRE(local.Contains(e));
            REQUIRE(local.Get(e) == order[k] * 10);
          }
        }
      }
    } while (std::next_permutation(order.begin(), order.end()));
  }

  SECTION("an entity that was never inserted changes nothing") {
    set.Emplace(a, 1);
    set.Emplace(b, 2);
    set.Remove(c);
    set.Remove({500, 0});
    set.Remove(EMPTY_ENTITY);

    REQUIRE(set.Size() == 2);
    REQUIRE(set.Get(a) == 1);
    REQUIRE(set.Get(b) == 2);
  }

  SECTION("a stale generation changes nothing") {
    set.Emplace({1, 5}, 7);
    set.Remove({1, 4});
    set.Remove({1, 6});

    REQUIRE(set.Size() == 1);
    REQUIRE(set.Contains({1, 5}));
    REQUIRE(set.Get({1, 5}) == 7);
  }

  SECTION("removing twice is harmless") {
    set.Emplace(a, 1);
    set.Emplace(b, 2);
    set.Remove(a);
    set.Remove(a);

    REQUIRE(set.Size() == 1);
    REQUIRE(set.Get(b) == 2);
  }

  SECTION("removing from an empty set is harmless") {
    set.Remove(a);

    REQUIRE(set.Size() == 0);
  }

  SECTION("an entity can be re-added after removal") {
    set.Emplace(a, 1);
    set.Emplace(b, 2);
    set.Remove(a);
    int* value = set.Emplace(a, 3);

    REQUIRE(value != nullptr);
    REQUIRE(*value == 3);
    REQUIRE(value == &set.Get(a));
    REQUIRE(set.Size() == 2);
    REQUIRE(set.Get(a) == 3);
    REQUIRE(set.Get(b) == 2);
  }

  SECTION("a newer generation can take the id of a removed entity") {
    Entity old_e{1, 0};
    Entity new_e{1, 1};
    set.Emplace(old_e, 1);
    set.Emplace(c, 3);
    set.Remove(old_e);
    int* value = set.Emplace(new_e, 2);

    REQUIRE(value != nullptr);
    REQUIRE(value == &set.Get(new_e));
    REQUIRE(set.Size() == 2);
    REQUIRE(set.Contains(new_e));
    REQUIRE_FALSE(set.Contains(old_e));
    REQUIRE(set.Get(new_e) == 2);
    REQUIRE(set.TryGet(old_e) == nullptr);
    REQUIRE(set.Get(c) == 3);
  }
}

TEST_CASE("SparseSet::Clear", "[sparseset]") {
  SparseSet<int> set;
  Entity a{0, 0};
  Entity b{9, 0};
  Entity c{300, 2};

  SECTION("on an empty set") {
    set.Clear();

    REQUIRE(set.Size() == 0);
    REQUIRE_FALSE(set.Contains(a));
  }

  SECTION("removes everything") {
    set.Emplace(a, 1);
    set.Emplace(b, 2);
    set.Emplace(c, 3);
    set.Clear();

    REQUIRE(set.Size() == 0);
    REQUIRE(set.GetEntities().empty());
    REQUIRE(set.GetData().empty());
    REQUIRE_FALSE(set.Contains(a));
    REQUIRE_FALSE(set.Contains(b));
    REQUIRE_FALSE(set.Contains(c));
    REQUIRE(set.TryGet(a) == nullptr);
    REQUIRE_THROWS_AS(set.Get(b), std::runtime_error);
  }

  SECTION("the set is usable afterwards") {
    set.Emplace(a, 1);
    set.Emplace(b, 2);
    set.Clear();

    set.Emplace(a, 10);
    set.Emplace({1, 0}, 11);
    set.Emplace(c, 30);

    REQUIRE(set.Size() == 3);
    REQUIRE(set.Get(a) == 10);
    REQUIRE(set.Get({1, 0}) == 11);
    REQUIRE(set.Get(c) == 30);
    REQUIRE_FALSE(set.Contains(b));
  }

  SECTION("clearing twice is harmless") {
    set.Emplace(a, 1);
    set.Clear();
    set.Clear();

    REQUIRE(set.Size() == 0);
    set.Emplace(a, 2);
    REQUIRE(set.Get(a) == 2);
  }
}

TEST_CASE("SparseSet::Size", "[sparseset]") {
  SparseSet<int> set;

  for (uint32_t i = 0; i < 50; ++i) {
    set.Emplace({i, 0}, static_cast<int>(i));
    REQUIRE(set.Size() == i + 1);
  }

  set.Emplace({10, 0}, 0);
  REQUIRE(set.Size() == 50);

  for (uint32_t i = 0; i < 50; i += 2) {
    set.Remove({i, 0});
  }
  REQUIRE(set.Size() == 25);

  set.Remove({0, 0});
  REQUIRE(set.Size() == 25);

  set.Clear();
  REQUIRE(set.Size() == 0);
}

TEST_CASE("SparseSet::GetEntities and GetData", "[sparseset]") {
  SparseSet<int> set;
  for (uint32_t i = 0; i < 10; ++i) {
    set.Emplace({i * 3, i}, static_cast<int>(i * 100));
  }

  SECTION("are parallel arrays") {
    auto entities = set.GetEntities();
    auto data = set.GetData();

    REQUIRE(entities.size() == set.Size());
    REQUIRE(data.size() == set.Size());

    for (size_t i = 0; i < entities.size(); ++i) {
      REQUIRE(set.Get(entities[i]) == data[i]);
      REQUIRE(&set.Get(entities[i]) == &data[i]);
    }
  }

  SECTION("stay parallel after removals") {
    set.Remove({0, 0});
    set.Remove({15, 5});
    set.Remove({27, 9});

    auto entities = set.GetEntities();
    auto data = set.GetData();

    REQUIRE(entities.size() == 7);
    REQUIRE(data.size() == 7);

    for (size_t i = 0; i < entities.size(); ++i) {
      REQUIRE(set.Contains(entities[i]));
      REQUIRE(data[i] == static_cast<int>(entities[i].gen * 100));
    }
  }

  SECTION("contain each entity exactly once") {
    std::unordered_set<uint32_t> seen;
    for (Entity e : set.GetEntities()) {
      REQUIRE(seen.insert(e.id).second);
    }
    REQUIRE(seen.size() == 10);
  }

  SECTION("writes through GetData are visible through Get") {
    for (int& value : set.GetData()) {
      value += 1;
    }

    for (uint32_t i = 0; i < 10; ++i) {
      REQUIRE(set.Get({i * 3, i}) == static_cast<int>(i * 100 + 1));
    }
  }
}

TEST_CASE("SparseSet::Get", "[sparseset]") {
  SparseSet<int> set;
  Entity a{0, 0};
  Entity b{4, 2};
  set.Emplace(a, 5);
  set.Emplace(b, 6);

  SECTION("returns the stored value") {
    REQUIRE(set.Get(a) == 5);
    REQUIRE(set.Get(b) == 6);
  }

  SECTION("returns a reference into the set") {
    set.Get(a) = 50;
    set.Get(b) += 4;

    REQUIRE(set.Get(a) == 50);
    REQUIRE(set.Get(b) == 10);
    REQUIRE(&set.Get(a) == set.TryGet(a));
  }

  SECTION("throws for missing entities") {
    REQUIRE_THROWS_AS(set.Get({1, 0}), std::runtime_error);
    REQUIRE_THROWS_AS(set.Get({4, 1}), std::runtime_error);
    REQUIRE_THROWS_AS(set.Get({4, 3}), std::runtime_error);
    REQUIRE_THROWS_AS(set.Get({99999, 0}), std::runtime_error);
    REQUIRE_THROWS_AS(set.Get(EMPTY_ENTITY), std::runtime_error);
  }

  SECTION("throws after removal") {
    set.Remove(a);

    REQUIRE_THROWS_AS(set.Get(a), std::runtime_error);
    REQUIRE_NOTHROW(set.Get(b));
  }
}

TEST_CASE("SparseSet::TryGet", "[sparseset]") {
  SparseSet<int> set;
  Entity a{0, 0};
  Entity b{4, 2};
  set.Emplace(a, 5);
  set.Emplace(b, 6);

  SECTION("returns a pointer to the stored value") {
    int* pa = set.TryGet(a);
    int* pb = set.TryGet(b);

    REQUIRE(pa != nullptr);
    REQUIRE(pb != nullptr);
    REQUIRE(*pa == 5);
    REQUIRE(*pb == 6);
    REQUIRE(pa != pb);
  }

  SECTION("writes through the pointer are visible") {
    *set.TryGet(b) = 60;

    REQUIRE(set.Get(b) == 60);
  }

  SECTION("returns nullptr for missing entities") {
    REQUIRE(set.TryGet({1, 0}) == nullptr);
    REQUIRE(set.TryGet({4, 1}) == nullptr);
    REQUIRE(set.TryGet({4, 3}) == nullptr);
    REQUIRE(set.TryGet({99999, 0}) == nullptr);
    REQUIRE(set.TryGet(EMPTY_ENTITY) == nullptr);
  }

  SECTION("returns nullptr after removal and clear") {
    set.Remove(b);
    REQUIRE(set.TryGet(b) == nullptr);
    REQUIRE(set.TryGet(a) != nullptr);

    set.Clear();
    REQUIRE(set.TryGet(a) == nullptr);
  }
}

TEST_CASE("SparseSet through the ISparseSet interface", "[sparseset]") {
  std::unique_ptr<ISparseSet> base = std::make_unique<SparseSet<int>>();
  auto& typed = static_cast<SparseSet<int>&>(*base);

  Entity a{0, 0};
  Entity b{1, 0};
  typed.Emplace(a, 1);
  typed.Emplace(b, 2);

  REQUIRE(base->Size() == 2);
  REQUIRE(base->Contains(a));
  REQUIRE(base->Contains(b));
  REQUIRE_FALSE(base->Contains({2, 0}));

  base->Remove(a);
  REQUIRE(base->Size() == 1);
  REQUIRE_FALSE(base->Contains(a));
  REQUIRE(typed.Get(b) == 2);

  base->Clear();
  REQUIRE(base->Size() == 0);
  REQUIRE_FALSE(base->Contains(b));
}

TEST_CASE("SparseSet manages element lifetimes", "[sparseset]") {
  Tracked::alive = 0;

  {
    std::unique_ptr<ISparseSet> base = std::make_unique<SparseSet<Tracked>>();
    auto& set = static_cast<SparseSet<Tracked>&>(*base);

    for (uint32_t i = 0; i < 5; ++i) {
      set.Emplace({i, 0}, static_cast<int>(i));
    }
    REQUIRE(Tracked::alive == 5);

    set.Remove({0, 0});
    REQUIRE(Tracked::alive == 4);
    REQUIRE(set.Get({4, 0}).value == 4);

    set.Remove({2, 0});
    REQUIRE(Tracked::alive == 3);

    set.Emplace({1, 0}, 100);
    REQUIRE(Tracked::alive == 3);
    REQUIRE(set.Get({1, 0}).value == 1);

    set.Clear();
    REQUIRE(Tracked::alive == 0);

    set.Emplace({0, 0}, 7);
    set.Emplace({1, 0}, 8);
    REQUIRE(Tracked::alive == 2);
  }

  REQUIRE(Tracked::alive == 0);
}

TEST_CASE(
  "SparseSet matches a reference model under random operations",
  "[sparseset][stress]"
) {
  constexpr uint32_t MAX_ID = 64;
  constexpr int STEPS = 20000;

  struct Slot {
    uint32_t gen;
    int value;
  };

  SparseSet<int> set;
  std::unordered_map<uint32_t, Slot> model;
  std::vector<uint32_t> generations(MAX_ID, 0);

  std::mt19937 rng(1337);
  std::uniform_int_distribution<uint32_t> pick_id(0, MAX_ID - 1);
  std::uniform_int_distribution<int> pick_op(0, 9);
  std::uniform_int_distribution<int> pick_value(-1000, 1000);

  for (int step = 0; step < STEPS; ++step) {
    uint32_t id = pick_id(rng);
    int op = pick_op(rng);
    Entity current{id, generations[id]};

    INFO(
      "step " << step << ", op " << op << ", id " << id << ", gen "
              << current.gen
    );

    if (op < 5) {
      int value = pick_value(rng);
      int* emplaced = set.Emplace(current, value);
      bool inserted = model.try_emplace(id, Slot{current.gen, value}).second;

      if (inserted) {
        REQUIRE(emplaced != nullptr);
        REQUIRE(*emplaced == value);
        REQUIRE(emplaced == set.TryGet(current));
      } else {
        REQUIRE(emplaced == nullptr);
      }
    } else if (op < 8) {
      set.Remove(current);
      if (model.erase(id) > 0) {
        ++generations[id];
      }
    } else if (op < 9) {
      set.Remove({id, current.gen + 1});
    } else if (step % 1000 == 999) {
      set.Clear();
      for (auto& [cleared_id, slot] : model) {
        ++generations[cleared_id];
      }
      model.clear();
    }

    REQUIRE(set.Size() == model.size());

    auto entities = set.GetEntities();
    auto data = set.GetData();
    REQUIRE(entities.size() == model.size());
    REQUIRE(data.size() == model.size());

    for (size_t i = 0; i < entities.size(); ++i) {
      auto it = model.find(entities[i].id);
      REQUIRE(it != model.end());
      REQUIRE(entities[i].gen == it->second.gen);
      REQUIRE(data[i] == it->second.value);
    }

    for (uint32_t check_id = 0; check_id < MAX_ID; ++check_id) {
      auto it = model.find(check_id);
      Entity live{check_id, generations[check_id]};

      if (it != model.end()) {
        REQUIRE(set.Contains(live));
        REQUIRE(set.Get(live) == it->second.value);
      } else {
        REQUIRE_FALSE(set.Contains(live));
        REQUIRE(set.TryGet(live) == nullptr);
      }

      if (live.gen > 0) {
        REQUIRE_FALSE(set.Contains({check_id, live.gen - 1}));
      }
    }
  }
}
