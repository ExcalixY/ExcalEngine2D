#include "ExcalCore/Layers/LayerStack.hpp"

#include <ranges>

namespace ExcalCore {
LayerStack::LayerStack(ApplicationSpec& spec)
    : m_Layers(0), m_InsertIndex(0) { }

LayerStack::~LayerStack() {
  for (Layer* l : m_Layers) {
    l->OnDetach();
    delete l;
  }
}

void LayerStack::PushLayer(Layer* layer) {
  m_Layers.emplace(m_Layers.begin() + m_InsertIndex++, layer);
  layer->OnAttach();
}

void LayerStack::PushOverlay(Layer* layer) {
  m_Layers.emplace_back(layer);
  layer->OnAttach();
}

void LayerStack::PopLayer(Layer* layer) {
  auto it = m_Layers.begin();

  for (size_t i = 0; i < m_InsertIndex; ++i) {
    if (*it == layer) {
      layer->OnDetach();

      m_Layers.erase(it);
      --m_InsertIndex;

      return;
    }

    ++it;
  }
}

void LayerStack::PopOverlay(Layer* layer) {
  auto it = m_Layers.begin();

  for (size_t i = m_InsertIndex; i < m_Layers.size(); ++i) {
    if (*it == layer) {
      layer->OnDetach();

      m_Layers.erase(it);
      return;
    }

    ++it;
  }
}

void LayerStack::OnUpdate(float dt) {
  for (auto layer : m_Layers) {
    layer->OnUpdate(dt);
  }
}
void LayerStack::OnRender(Renderer2D& renderer) {
  for (auto layer : m_Layers) {
    layer->OnRender(renderer);
  }
}

std::vector<Layer*>::iterator LayerStack::begin() {
  return m_Layers.begin();
}
std::vector<Layer*>::iterator LayerStack::end() {
  return m_Layers.end();
}

std::vector<Layer*>::reverse_iterator LayerStack::rbegin() {
  return m_Layers.rbegin();
}
std::vector<Layer*>::reverse_iterator LayerStack::rend() {
  return m_Layers.rend();
}

}
