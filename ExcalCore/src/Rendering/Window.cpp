#include "ExcalCore/Rendering/Window.hpp"

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

Window::Window(ApplicationSpec& spec) : m_Spec(spec) {
  Debug::Log("Initializing Window...");

  SDL_WindowFlags fs_mode;

  switch (m_Spec.windowSpec.fullscreenState) {
    case FullscreenState::BORDERLESS:
      fs_mode = SDL_WINDOW_BORDERLESS;
      break;
    case FullscreenState::WINDOWED:
      fs_mode = SDL_WINDOW_RESIZABLE;
      break;
    case FullscreenState::FULLSCREEN:
      fs_mode = SDL_WINDOW_FULLSCREEN;
      break;
  }

  m_Window = SDL_CreateWindow(
    spec.name.c_str(), spec.windowSpec.size.x, spec.windowSpec.size.y, fs_mode
  );

  if (m_Window == nullptr) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE, "Failed to initial SDL Window: {}", SDL_GetError()
    );
  }
}

Window::~Window() {
  Debug::Log("Destroying Window...");

  SDL_DestroyWindow(m_Window);
}

void Window::OnEvent(const WindowResizeEvent& e) {
  Debug::Log("Resized window: {}:{}", e.width, e.height);
}

}
