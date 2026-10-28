#include "ExcalCore/Objects/Components/Component.hpp"

namespace ExcalCore {

GameObject* ComponentBase::GetOwner() const {
  return m_Owner;
}

bool ComponentBase::IsEnabled() const {
  return m_Enabled;
}

void ComponentBase::SetEnabled(bool enabled) {
  if (m_Enabled == enabled) {
    return;
  }

  m_Enabled = enabled;

  if (m_Enabled) {
    OnEnabled();
  } else {
    OnDisabled();
  }
}

}
