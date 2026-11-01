#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <utility>
#include <vector>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/Event.hpp"
#include "ExcalCore/Events/EventHandle.hpp"
#include "ExcalCore/Systems/EventSystem.hpp"

using namespace ExcalCore;

namespace {

struct TestEvent : Event {
  int value = 0;
};

struct OtherEvent : Event { };

}

TEST_CASE("EventSystem delivers immediate events to subscribers", "[events]") {
  ApplicationSpec spec;
  EventSystem events(spec);

  SECTION("a subscriber receives the event data") {
    int received = -1;
    EventHandle handle = events.Subscribe<TestEvent>([&](TestEvent& e) {
      received = e.value;
      return false;
    });

    TestEvent e{.value = 42};
    events.Dispatch<TestEvent, true>(e);

    REQUIRE(received == 42);
  }

  SECTION("every subscriber is called, in subscription order") {
    std::vector<int> order;
    EventHandle a = events.Subscribe<TestEvent>([&](TestEvent&) {
      order.push_back(1);
      return false;
    });
    EventHandle b = events.Subscribe<TestEvent>([&](TestEvent&) {
      order.push_back(2);
      return false;
    });
    EventHandle c = events.Subscribe<TestEvent>([&](TestEvent&) {
      order.push_back(3);
      return false;
    });

    TestEvent e;
    events.Dispatch<TestEvent, true>(e);

    REQUIRE(order == std::vector<int>{1, 2, 3});
  }

  SECTION("subscribers only receive their own event type") {
    int test_calls = 0;
    EventHandle handle = events.Subscribe<TestEvent>([&](TestEvent&) {
      ++test_calls;
      return false;
    });

    OtherEvent other;
    events.Dispatch<OtherEvent, true>(other);

    REQUIRE(test_calls == 0);
  }
}

TEST_CASE("EventHandle controls the subscription lifetime", "[events]") {
  ApplicationSpec spec;
  EventSystem events(spec);

  int calls = 0;
  auto callback = [&](TestEvent&) {
    ++calls;
    return false;
  };

  SECTION("destroying the handle unsubscribes") {
    { EventHandle handle = events.Subscribe<TestEvent>(callback); }

    TestEvent e;
    events.Dispatch<TestEvent, true>(e);

    REQUIRE(calls == 0);
  }

  SECTION("Unsubscribe stops delivery and is safe to repeat") {
    EventHandle handle = events.Subscribe<TestEvent>(callback);
    handle.Unsubscribe();
    handle.Unsubscribe();

    TestEvent e;
    events.Dispatch<TestEvent, true>(e);

    REQUIRE(calls == 0);
  }

  SECTION("a moved handle keeps the subscription alive") {
    std::optional<EventHandle> moved;
    {
      EventHandle original = events.Subscribe<TestEvent>(callback);
      moved.emplace(std::move(original));
    }

    TestEvent e;
    events.Dispatch<TestEvent, true>(e);

    REQUIRE(calls == 1);
  }

  SECTION("removing one subscriber leaves the others intact") {
    int other_calls = 0;
    EventHandle keep = events.Subscribe<TestEvent>([&](TestEvent&) {
      ++other_calls;
      return false;
    });

    { EventHandle first = events.Subscribe<TestEvent>(callback); }
    { EventHandle second = events.Subscribe<TestEvent>(callback); }

    TestEvent e;
    events.Dispatch<TestEvent, true>(e);

    REQUIRE(other_calls == 1);
    REQUIRE(calls == 0);
  }
}
