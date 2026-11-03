#include "ExcalCore/Systems/ImGUISystem.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_internal.h>

#include "ExcalCore/ApplicationSpec.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"
#include "ExcalCore/Rendering/Window.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

namespace {

unsigned char DEFAULT_FONT_DATA[] = {
#embed "../../res/Fonts/JetBrainsMono-Regular.ttf"
};

constexpr float DEFAULT_FONT_SIZE = 16.0f;

}

ImGUISystem* ImGUISystem::instance = nullptr;

ImGUISystem::ImGUISystem(
  ApplicationSpec& spec, Window& window, Renderer2D& renderer
) {
  if (instance) {
    Debug::ThrowError(
      Debug::CATEGORY_RENDER,
      "Attempted to initialize a second instance of ImGUI System."
    );
  }

  instance = this;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImFontConfig font_config;
  font_config.FontDataOwnedByAtlas = false;
  io.Fonts->AddFontFromMemoryTTF(
    DEFAULT_FONT_DATA,
    sizeof(DEFAULT_FONT_DATA),
    DEFAULT_FONT_SIZE,
    &font_config
  );

  ImGui::StyleColorsDark();

  ImGui_ImplSDL3_InitForSDLRenderer(
    window.GetWindowRaw(), renderer.GetRendererRaw()
  );
  ImGui_ImplSDLRenderer3_Init(renderer.GetRendererRaw());
}

ImGUISystem::~ImGUISystem() {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  instance = nullptr;
}

void ImGUISystem::BeginRender() {
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
}

void ImGUISystem::OnRender(Renderer2D& renderer) {
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(
    ImGui::GetDrawData(), renderer.GetRendererRaw()
  );
}

void ImGUISystem::OnFrameEnd() { }

void ImGUISystem::ProcessEvent(SDL_Event* e) {
  ImGui_ImplSDL3_ProcessEvent(e);
}

}
