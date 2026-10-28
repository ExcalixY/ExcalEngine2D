#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/WindowResizeEvent.hpp"

namespace ExcalCore {

class Window final {
public:
  explicit Window(ApplicationSpec& spec);
  ~Window();

  SDL_Window* GetWindowRaw() {
    return m_Window;
  }

  void OnEvent(const WindowResizeEvent& e);

private:
  ApplicationSpec& m_Spec;
  SDL_Window* m_Window;
};

}
