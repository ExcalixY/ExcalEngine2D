#pragma once

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Rendering/Window.hpp"
#include "SDL3/SDL_events.h"

namespace ExcalCore {

class ImGUISystem {
public:
  explicit ImGUISystem(
    ApplicationSpec& spec, Window& window, Renderer2D& renderer
  );
  ~ImGUISystem();

  void ProcessEvent(SDL_Event* e);

  void OnInit() { }
  void BeginRender();
  void OnUpdate() { }
  void OnLateUpdate() { }
  void OnRender(Renderer2D& renderer);
  void OnFrameEnd();

private:
  static ImGUISystem* instance;
};
}
