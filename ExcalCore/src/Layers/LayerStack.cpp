#include "ExcalCore/Layers/LayerStack.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Layers/Layer.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

LayerStack* LayerStack::instance = nullptr;

LayerStack::LayerStack(ApplicationSpec& spec) {
  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Attempted to initialize a second instance of LayerStack."
    );
  }

  instance = this;
}

LayerStack::~LayerStack() {
  for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it) {
    (*it)->OnDetach();
  }

  instance = nullptr;
}

LayerStack* LayerStack::GetInstance() {
  if (!instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE, "LayerStack has not been initialized."
    );
  }

  return instance;
}

void LayerStack::PushLayer(std::unique_ptr<Layer> layer) {
  Layer* raw = layer.get();

  m_Layers.emplace(
    m_Layers.begin() + static_cast<std::ptrdiff_t>(m_InsertIndex++),
    std::move(layer)
  );
  raw->OnAttach();
}

void LayerStack::PushOverlay(std::unique_ptr<Layer> layer) {
  Layer* raw = layer.get();

  m_Layers.emplace_back(std::move(layer));
  raw->OnAttach();
}

void LayerStack::PopLayer(Layer* layer) {
  auto layers_end =
    m_Layers.begin() + static_cast<std::ptrdiff_t>(m_InsertIndex);

  auto it = std::find_if(m_Layers.begin(), layers_end, [layer](auto& l) {
    return l.get() == layer;
  });

  if (it == layers_end) {
    return;
  }

  layer->OnDetach();
  m_Layers.erase(it);
  --m_InsertIndex;
}

void LayerStack::PopOverlay(Layer* layer) {
  auto overlays_begin =
    m_Layers.begin() + static_cast<std::ptrdiff_t>(m_InsertIndex);

  auto it = std::find_if(overlays_begin, m_Layers.end(), [layer](auto& l) {
    return l.get() == layer;
  });

  if (it == m_Layers.end()) {
    return;
  }

  layer->OnDetach();
  m_Layers.erase(it);
}

void LayerStack::OnStart() {
  for (auto& layer : m_Layers) {
    layer->OnStart();
  }
}

void LayerStack::OnUpdate(float dt) {
  for (auto& layer : m_Layers) {
    layer->OnUpdate(dt);
  }
}
void LayerStack::OnRender(Renderer2D& renderer) {
  for (auto& layer : m_Layers) {
    layer->OnRender(renderer);
  }
}

std::vector<std::unique_ptr<Layer>>::iterator LayerStack::begin() {
  return m_Layers.begin();
}
std::vector<std::unique_ptr<Layer>>::iterator LayerStack::end() {
  return m_Layers.end();
}

std::vector<std::unique_ptr<Layer>>::reverse_iterator LayerStack::rbegin() {
  return m_Layers.rbegin();
}
std::vector<std::unique_ptr<Layer>>::reverse_iterator LayerStack::rend() {
  return m_Layers.rend();
}

}
