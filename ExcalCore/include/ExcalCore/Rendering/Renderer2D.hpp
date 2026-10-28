#pragma once

#include <SDL3/SDL.h>

#include <unordered_map>
#include <vector>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/WindowResizeEvent.hpp"
#include "ExcalCore/Rendering/DrawCommand.hpp"
#include "ExcalCore/Rendering/Window.hpp"

namespace ExcalCore {

class Renderer2D final {
public:
  explicit Renderer2D(ApplicationSpec& spec, Window& window);
  ~Renderer2D();

  SDL_Renderer* GetRendererRaw() {
    return m_Renderer;
  }

  void BeginFrame();
  void SubmitDrawCommand(const DrawCommand& command);
  void EndFrame();

  SDL_Texture* GetTextureFromSurface(SDL_Surface* surface);

  void OnEvent(const WindowResizeEvent& e);

private:
  ApplicationSpec& m_Spec;
  SDL_Renderer* m_Renderer;

  std::unordered_map<SDL_Surface*, std::shared_ptr<SDL_Texture>> m_Textures;

  std::vector<DrawCommand> m_CommandBuffer;
};

}
