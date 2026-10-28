#pragma once

#include <string>

#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore {

class Layer {
public:
  explicit Layer(std::string name) : m_Name(std::move(name)) { }
  virtual ~Layer() = default;

  virtual void OnAttach() { }
  virtual void OnDetach() { }

  virtual void OnStart() { }
  virtual void OnUpdate(float dt) { }
  virtual void OnRender(Renderer2D& renderer) { }

  virtual void OnEvent() { }

  const std::string& GetName() const {
    return m_Name;
  }

private:
  std::string m_Name;
};

}
