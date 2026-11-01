#include "ExcalCore/Rendering/Renderer2D.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_render.h>

#include <algorithm>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Events/EventTypes/WindowResizeEvent.hpp"
#include "ExcalCore/Rendering/DrawCommand.hpp"
#include "ExcalCore/Rendering/Window.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

Renderer2D* Renderer2D::instance = nullptr;

Renderer2D::Renderer2D(ApplicationSpec& spec, Window& window) : m_Spec(spec) {
  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE,
      "Attempted to initialize a second instance of Renderer2D."
    );
  }

  instance = this;

  Debug::Log("Initializing 2D Renderer...");

  m_Renderer = SDL_CreateRenderer(window.GetWindowRaw(), nullptr);

  if (m_Renderer == nullptr) {
    Debug::ThrowError(
      Debug::CATEGORY_RENDER,
      "Failed to initialize Renderer2D: {}",
      SDL_GetError()
    );
  }

  SDL_SetRenderVSync(m_Renderer, spec.rendererSpec.vsync);
}

Renderer2D* Renderer2D::GetInstance() {
  if (!instance) {
    Debug::ThrowError(
      Debug::CATEGORY_ENGINE, "Renderer2D has not been initialized."
    );
  }

  return instance;
}

SDL_Renderer* Renderer2D::GetRendererRaw() {
  return m_Renderer;
}

// SDL_Texture* Renderer2D::GetTextureFromSurface(SDL_Surface* surface) {
//   auto it = m_Textures.find(surface);
//
//   if (it == m_Textures.end()) {
//     SDL_Texture* texture = SDL_CreateTextureFromSurface(m_Renderer, surface);
//
//     if (!texture) {
//       Debug::LogError(
//         Debug::CATEGORY_RENDER,
//         "Failed to create Texture from Surface: {}",
//         SDL_GetError()
//       );
//       return nullptr;
//     }
//
//     m_Textures[surface] =
//       std::shared_ptr<SDL_Texture>(texture, SDL_DestroyTexture);
//
//     return texture;
//   } else {
//     return it->second.get();
//   }
//
//   return nullptr;
// }

void Renderer2D::BeginFrame() {
  SDL_RenderClear(m_Renderer);
  m_CommandBuffer.clear();
}

void Renderer2D::SubmitDrawCommand(const DrawCommand& command) {
  m_CommandBuffer.push_back(command);
}

void Renderer2D::EndFrame() {
  std::ranges::stable_sort(m_CommandBuffer, {}, &DrawCommand::layer);

  for (auto& command : m_CommandBuffer) {
    if (!command.texture) {
      Debug::LogError(
        Debug::CATEGORY_RENDER, "Missing Texture in Draw Command."
      );

      continue;
    }

    // if (!command.texture->GetSurface()) {
    //   Debug::LogError(
    //     Debug::CATEGORY_RENDER,
    //     "Attempted to Render a TextureAsset without a Surface attached."
    //   );

    //   continue;
    // }

    // SDL_Texture* texture =
    // GetTextureFromSurface(command.texture->GetSurface());

    SDL_Texture* texture = command.texture->GetTexture();
    if (!texture) {
      texture =
        SDL_CreateTextureFromSurface(m_Renderer, command.texture->GetSurface());

      command.texture->SetTexture(texture);
    }

    if (!texture) {
      Debug::LogError(
        Debug::CATEGORY_RENDER,
        "Failed to acquire Texture from Surface: {}",
        SDL_GetError()
      );

      continue;
    }

    SDL_SetTextureColorModFloat(
      texture, command.color.r, command.color.g, command.color.b
    );
    SDL_SetTextureAlphaModFloat(texture, command.color.a);

    bool call = SDL_RenderTextureRotated(
      m_Renderer,
      texture,
      command.source.has_value() ? &*command.source : nullptr,
      &command.destination,
      command.rotation,
      command.pivot.has_value() ? &*command.pivot : nullptr,
      command.flip
    );

    if (!call) {
      Debug::LogError(
        Debug::CATEGORY_RENDER,
        "Failed to render Texture to Renderer: {}",
        SDL_GetError()
      );
    }
  }

  SDL_RenderPresent(m_Renderer);
}

Renderer2D::~Renderer2D() {
  Debug::Log("Destroying Renderer2D...");

  // m_Textures.clear();
  SDL_DestroyRenderer(m_Renderer);
}

void Renderer2D::OnEvent(WindowResizeEvent& e) { }

}
