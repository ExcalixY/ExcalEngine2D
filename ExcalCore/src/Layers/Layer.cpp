#include "ExcalCore/Layers/Layer.hpp"

#include <utility>

namespace ExcalCore {

Layer::Layer(std::string name) : m_Name(std::move(name)) { }

const std::string& Layer::GetName() const {
  return m_Name;
}

}
