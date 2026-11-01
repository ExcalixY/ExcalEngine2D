#pragma once

#include <SDL3/SDL_video.h>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/EventTypes/WindowResizeEvent.hpp"

namespace ExcalCore {

class Window final {
public:
  explicit Window(ApplicationSpec& spec);
  ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  [[nodiscard]]
  static Window* GetInstance();

  [[nodiscard]]
  SDL_Window* GetWindowRaw();

  void OnEvent(WindowResizeEvent& e);

private:
  static Window* instance;

  ApplicationSpec& m_Spec;
  SDL_Window* m_Window;
};

}
