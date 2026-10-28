#include "ExcalCore/Application.hpp"

#include <SDL3/SDL.h>

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

Application::Application(ApplicationSpec spec)
    : m_Spec(spec),
      m_Initializer(m_Spec),
      m_Window(m_Spec),
      m_Renderer(m_Spec, m_Window),
      m_Assets(m_Spec),
      m_Time(m_Spec),
      m_Input(m_Spec),
      m_Layers(m_Spec),
      m_Events(m_Spec) { }

void Application::Run() {
  Debug::Log("Starting Runtime Loop...");

  m_Running = true;

  m_Time.Reset();

  while (m_Running) {
    // Update Time
    m_Time.Tick();

    // Update Events
    m_Input.BeginFrame();
    m_Events.PollEvents([&](const auto& event) {
      m_Events.DispatchEvent(event, *this, m_Window, m_Renderer, m_Input);
    });

    // Update Loop
    m_Layers.OnUpdate(m_Time.GetDeltaTime());

    // Rendering Loop
    m_Renderer.BeginFrame();
    m_Layers.OnRender(m_Renderer);
    m_Renderer.EndFrame();
  }
}

Application::~Application() { }

const ApplicationSpec& Application::GetSpec() const {
  return m_Spec;
}

Window& Application::GetWindow() {
  return m_Window;
}

Renderer2D& Application::GetRenderer() {
  return m_Renderer;
}

AssetManager& Application::GetAssets() {
  return m_Assets;
}

TimeSystem& Application::GetTime() {
  return m_Time;
}

InputSystem& Application::GetInput() {
  return m_Input;
}

}
