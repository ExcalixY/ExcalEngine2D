#pragma once

#include <SDL3/SDL_render.h>

// #include <unordered_map>
#include <vector>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/EventTypes/WindowResizeEvent.hpp"
#include "ExcalCore/Rendering/DrawCommand.hpp"
#include "ExcalCore/Rendering/Window.hpp"

namespace ExcalCore {

class Renderer2D final {
public:
  explicit Renderer2D(ApplicationSpec& spec, Window& window);
  ~Renderer2D();

  Renderer2D(const Renderer2D&) = delete;
  Renderer2D& operator=(const Renderer2D&) = delete;

  [[nodiscard]]
  static Renderer2D* GetInstance();

  [[nodiscard]]
  SDL_Renderer* GetRendererRaw();

  void BeginFrame();
  void SubmitDrawCommand(const DrawCommand& command);
  void EndFrame();

  // [[nodiscard]]
  // SDL_Texture* GetTextureFromSurface(SDL_Surface* surface);

  void OnEvent(WindowResizeEvent& e);

private:
  static Renderer2D* instance;

  ApplicationSpec& m_Spec;
  SDL_Renderer* m_Renderer;

  // std::unordered_map<SDL_Surface*, std::weak_ptr<SDL_Texture>> m_Textures;

  std::vector<DrawCommand> m_CommandBuffer;
};

}
