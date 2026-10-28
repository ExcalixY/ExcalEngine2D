#pragma once

#include <SDL3/SDL.h>

#include <unordered_map>

#include "ExcalCore/ApplicationSpec.hpp"
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

  void SubmitDrawCommand(const DrawCommand& command);

  SDL_Texture* GetTextureFromSurface(SDL_Surface* surface);

private:
  ApplicationSpec& m_Spec;
  SDL_Renderer* m_Renderer;

  std::unordered_map<SDL_Surface*, std::shared_ptr<SDL_Texture>> m_Textures;
};

}
