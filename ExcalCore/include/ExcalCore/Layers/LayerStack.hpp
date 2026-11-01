#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "Layer.hpp"

namespace ExcalCore {

class LayerStack {
public:
  explicit LayerStack(ApplicationSpec& spec);
  ~LayerStack();

  LayerStack(const LayerStack&) = delete;
  LayerStack& operator=(const LayerStack&) = delete;

  [[nodiscard]]
  static LayerStack* GetInstance();

  void PushLayer(std::unique_ptr<Layer> layer);
  void PushOverlay(std::unique_ptr<Layer> layer);

  void PopLayer(Layer* layer);
  void PopOverlay(Layer* layer);

  void OnStart();
  void OnUpdate(float dt);
  void OnRender(Renderer2D& renderer);

  std::vector<std::unique_ptr<Layer>>::iterator begin();
  std::vector<std::unique_ptr<Layer>>::iterator end();

  std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin();
  std::vector<std::unique_ptr<Layer>>::reverse_iterator rend();

private:
  static LayerStack* instance;

  std::vector<std::unique_ptr<Layer>> m_Layers;
  size_t m_InsertIndex = 0;
};

}
