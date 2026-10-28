#pragma once

#include <vector>

#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "Layer.hpp"

namespace ExcalCore {

class LayerStack {
public:
  explicit LayerStack();
  ~LayerStack();

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* layer);

  void PopLayer(Layer* layer);
  void PopOverlay(Layer* layer);

  void OnUpdate(float dt);
  void OnRender(Renderer2D& renderer);

  std::vector<Layer*>::iterator begin();
  std::vector<Layer*>::iterator end();

  std::vector<Layer*>::reverse_iterator rbegin();
  std::vector<Layer*>::reverse_iterator rend();

private:
  std::vector<Layer*> m_Layers;
  size_t m_InsertIndex;
};

}
