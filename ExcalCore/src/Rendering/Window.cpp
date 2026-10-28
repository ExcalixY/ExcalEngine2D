#include "ExcalCore/Rendering/Window.hpp"

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

Window::Window(ApplicationSpec& spec) : m_Spec(spec) {
  Debug::Log("Initializing Window...");

  m_Window = SDL_CreateWindow(
    spec.name.c_str(),
    spec.windowSpec.size.x,
    spec.windowSpec.size.y,
    SDL_WINDOW_FULLSCREEN
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

}
