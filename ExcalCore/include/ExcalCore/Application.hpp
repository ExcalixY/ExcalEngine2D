#pragma once

#include <concepts>

#include "Events/EventTypes/WindowShouldCloseEvent.hpp"
#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/EngineInitializer.hpp"
#include "ExcalCore/Layers/Layer.hpp"
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

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  [[nodiscard]]
  static Application* GetInstance();

  void Run();

  template <typename T, typename... TArgs>
    requires std::derived_from<T, Layer>
  void PushLayer(TArgs&&... args);

  template <typename T, typename... TArgs>
    requires std::derived_from<T, Layer>
  void PushOverlay(TArgs&&... args);

  void OnEvent(WindowShouldCloseEvent& e);

  [[nodiscard]]
  const ApplicationSpec& GetSpec() const;

  [[nodiscard]]
  Window& GetWindow();
  [[nodiscard]]
  Renderer2D& GetRenderer();

  [[nodiscard]]
  AssetManager& GetAssets();
  [[nodiscard]]
  TimeSystem& GetTime();
  [[nodiscard]]
  InputSystem& GetInput();

private:
  static Application* instance;

  ApplicationSpec m_Spec;

  // Core Systems
  EngineInitializer m_Initializer;
  Window m_Window;
  Renderer2D m_Renderer;
  AssetManager m_Assets;
  TimeSystem m_Time;
  InputSystem m_Input;

  EventSystem m_Events;

  LayerStack m_Layers;

  bool m_Running = false;
};

template <typename T, typename... TArgs>
  requires std::derived_from<T, Layer>
void Application::PushLayer(TArgs&&... args) {
  m_Layers.PushLayer(std::make_unique<T>(std::forward<TArgs>(args)...));
}

template <typename T, typename... TArgs>
  requires std::derived_from<T, Layer>
void Application::PushOverlay(TArgs&&... args) {
  m_Layers.PushOverlay(std::make_unique<T>(std::forward<TArgs>(args)...));
}

}
