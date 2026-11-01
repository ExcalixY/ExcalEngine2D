#include "ExcalCore/Rendering/Window.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_video.h>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/EventTypes/WindowResizeEvent.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

Window* Window::instance = nullptr;

Window::Window(ApplicationSpec& spec) : m_Spec(spec) {
  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Attempted to initialize a second instance of Window."
    );
  }

  instance = this;

  Debug::Log("Initializing Window...");

  SDL_WindowFlags fs_mode = 0;

  switch (m_Spec.windowSpec.fullscreenState) {
    case FullscreenState::BORDERLESS:
      fs_mode = SDL_WINDOW_BORDERLESS;
      fs_mode |= SDL_WINDOW_FULLSCREEN;
      break;
    case FullscreenState::WINDOWED:
      break;
    case FullscreenState::FULLSCREEN:
      fs_mode = SDL_WINDOW_FULLSCREEN;
      break;
  }

  fs_mode |= (spec.windowSpec.allowResize ? SDL_WINDOW_RESIZABLE : 0);
  fs_mode |= (spec.windowSpec.alwaysOnTop ? SDL_WINDOW_ALWAYS_ON_TOP : 0);
  fs_mode |=
    (spec.windowSpec.startMaximized ? SDL_WINDOW_MAXIMIZED
                                    : SDL_WINDOW_MINIMIZED);

  m_Window = SDL_CreateWindow(
    spec.name.c_str(), spec.windowSpec.size.x, spec.windowSpec.size.y, fs_mode
  );

  if (m_Window == nullptr) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Failed to initialize SDL Window: {}",
      SDL_GetError()
    );
  }

  if (m_Spec.windowSpec.fullscreenState == FullscreenState::FULLSCREEN) {
    SDL_DisplayID display = SDL_GetDisplayForWindow(m_Window);
    const SDL_DisplayMode* mode_cur = SDL_GetDesktopDisplayMode(display);

    SDL_DisplayMode mode;
    if (
      SDL_GetClosestFullscreenDisplayMode(
        display, mode_cur->w, mode_cur->h, 0.f, false, &mode
      )
    ) {
      SDL_SetWindowFullscreenMode(m_Window, &mode);
      SDL_SetWindowFullscreen(m_Window, true);
    }
  }
}

Window::~Window() {
  Debug::Log("Destroying Window...");

  SDL_DestroyWindow(m_Window);
  instance = nullptr;
}

Window* Window::GetInstance() {
  if (!instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE, "Window has not been initialized."
    );
  }

  return instance;
}

SDL_Window* Window::GetWindowRaw() {
  return m_Window;
}

void Window::OnEvent(WindowResizeEvent& e) {
  Debug::Log("Resized window: {}:{}", e.width, e.height);
}

}
