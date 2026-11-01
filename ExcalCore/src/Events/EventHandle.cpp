#include "ExcalCore/Events/EventHandle.hpp"

#include "ExcalCore/Systems/EventSystem.hpp"

namespace ExcalCore {

void EventHandle::Unsubscribe() {
  if (tagID == INVALID_TAG) {
    return;
  }

  Events::Unsubscribe(*this);
  tagID = INVALID_TAG;
}

}
