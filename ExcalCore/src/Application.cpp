#include "ExcalCore/Application.hpp"

#include <SDL3/SDL.h>

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

Application::Application(ApplicationSpec spec)
    : m_Spec(spec),
      m_Initializer(),
      m_Window(m_Spec),
      m_Renderer(m_Spec, m_Window),
      m_Time(m_Spec) { }

void Application::Run() {
  Debug::Log("Starting Runtime Loop...");

  m_Running = true;

  m_Time.Reset();

  SDL_Event e;

  while (m_Running) {
    m_Time.Tick();

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        m_Running = false;
      }
    }

    // Update Loop
    m_Layers.OnUpdate(m_Time.GetDeltaTime());

    // Rendering Loop
    SDL_RenderClear(m_Renderer.GetRendererRaw());
    m_Layers.OnRender(m_Renderer);
    SDL_RenderPresent(m_Renderer.GetRendererRaw());
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
