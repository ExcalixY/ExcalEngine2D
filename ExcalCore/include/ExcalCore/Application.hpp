#pragma once

#include <concepts>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/EngineInitializer.hpp"
#include "ExcalCore/Layers/LayerStack.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Rendering/Window.hpp"
#include "ExcalCore/Systems/AssetManager.hpp"
#include "ExcalCore/Systems/EventSystem.hpp"
#include "ExcalCore/Systems/InputSystem.hpp"
#include "ExcalCore/Systems/TimeSystem.hpp"

namespace ExcalCore {

class Application {
public:
  explicit Application(ApplicationSpec spec);
  ~Application();

  void Run();

  template <typename T, typename... TArgs>
    requires std::derived_from<T, Layer>
  void PushLayer(TArgs&&... args) {
    m_Layers.PushLayer(new T(std::forward<TArgs>(args)...));
  }

  template <typename T, typename... TArgs>
    requires std::derived_from<T, Layer>
  void PushOverlay(TArgs&&... args) {
    m_Layers.PushOverlay(new T(std::forward<TArgs>(args)...));
  }

  const ApplicationSpec& GetSpec() const;

  Window& GetWindow();
  Renderer2D& GetRenderer();

  AssetManager& GetAssets();
  TimeSystem& GetTime();
  InputSystem& GetInput();

private:
  ApplicationSpec m_Spec;

  EngineInitializer m_Initializer;

  EventSystem m_Events;

  Window m_Window;
  Renderer2D m_Renderer;

  AssetManager m_Assets;
  TimeSystem m_Time;
  InputSystem m_Input;

  LayerStack m_Layers;

  bool m_Running;
};

}
