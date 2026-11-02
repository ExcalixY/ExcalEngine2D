#include "ExcalCore/Application.hpp"

#include <imgui.h>

#include <concepts>
#include <type_traits>
#include <utility>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/EventTypes/InputEvents.hpp"
#include "ExcalCore/Events/EventTypes/KeyEvents.hpp"
#include "ExcalCore/Events/EventTypes/MouseEvents.hpp"
#include "ExcalCore/Events/EventTypes/WindowShouldCloseEvent.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Rendering/Window.hpp"
#include "ExcalCore/Systems/AssetManager.hpp"
#include "ExcalCore/Systems/InputSystem.hpp"
#include "ExcalCore/Systems/TimeSystem.hpp"
#include "ExcalCore/Utils/Logging.hpp"
#include "SDL3/SDL_events.h"

namespace ExcalCore {

Application* Application::instance = nullptr;

Application::Application(ApplicationSpec spec)
    : m_Spec(std::move(spec)),
      m_Initializer(m_Spec),
      m_Window(m_Spec),
      m_Renderer(m_Spec, m_Window),
      m_Assets(m_Spec),
      m_Time(m_Spec),
      m_Input(m_Spec),
      m_Events(m_Spec),
      m_Layers(m_Spec),
      m_ImGUI(m_Spec, m_Window, m_Renderer) {
  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Attempted to initialize a second instance of Application."
    );
  }

  instance = this;
}

void Application::Run() {
  Debug::Log("Starting Runtime Loop...");

  m_Running = true;
  m_Time.Reset();

  m_Layers.OnApplicationStart();

  while (m_Running) {
    // Update Time
    m_Time.Tick();

    // Update Events
    m_Input.OnFrameStart();

    m_Layers.OnFrameStart();
    m_ImGUI.OnFrameStart();

    // Events
    m_Events.PollEvents([&](auto&& event, SDL_Event* sdl_event) {
      m_ImGUI.ProcessEvent(sdl_event);

      using T = std::remove_cvref_t<decltype(event)>;

      auto& io = ImGui::GetIO();

      constexpr bool is_release = std::same_as<T, KeyReleasedEvent> ||
                                  std::same_as<T, MouseButtonReleasedEvent>;

      if constexpr (!is_release) {
        if constexpr (std::derived_from<T, MouseEvent>) {
          if (io.WantCaptureMouse) {
            return;
          }
        } else if constexpr (std::derived_from<T, KeyboardEvent>) {
          if (io.WantCaptureKeyboard) {
            return;
          }
        }
      }

      m_Events.DispatchEvent(event, *this, m_Window, m_Renderer, m_Input);
    });
    m_Events.UpdateEvents();

    // Update Loop
    m_Layers.OnUpdate(m_Time.GetDeltaTime());

    // Rendering Loop
    m_Renderer.BeginFrame();
    m_Layers.OnRender(m_Renderer);
    m_Renderer.ProcessDrawCommands();
    m_ImGUI.OnRender(m_Renderer);
    m_Renderer.EndFrame();

    m_ImGUI.OnFrameEnd();

    m_Layers.OnFrameEnd();
  }
}

Application::~Application() {
  instance = nullptr;
}

Application* Application::GetInstance() {
  if (!instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE, "Application has not been initialized."
    );
  }

  return instance;
}

void Application::OnEvent(WindowShouldCloseEvent& e) {
  m_Running = false;
}

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
