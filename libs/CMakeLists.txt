# Everything is linked statically so the game ships as a single executable
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# SDL 3
set(SDL_SHARED OFF CACHE BOOL "Build shared SDL library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build static SDL library" FORCE)

add_subdirectory(SDL)

# SDL_ttf (vendored FreeType only, no HarfBuzz / PlutoSVG)
set(SDLTTF_VENDORED ON CACHE BOOL "Use vendored third-party libraries" FORCE)
set(SDLTTF_HARFBUZZ OFF CACHE BOOL "Use harfbuzz to improve text shaping" FORCE)
set(SDLTTF_PLUTOSVG OFF CACHE BOOL "Use plutosvg for color emoji support" FORCE)
set(SDLTTF_SAMPLES OFF CACHE BOOL "Build the SDL3_ttf sample program(s)" FORCE)
set(SDLTTF_INSTALL OFF CACHE BOOL "Enable SDL3_ttf install target" FORCE)

add_subdirectory(SDL_ttf)

# SDL_image (built-in stb_image for PNG / JPG, no external codecs)
set(SDLIMAGE_VENDORED ON CACHE BOOL "Use vendored third-party libraries" FORCE)
set(SDLIMAGE_BACKEND_STB ON CACHE BOOL "Use stb_image for loading JPEG files" FORCE)
set(SDLIMAGE_PNG_LIBPNG OFF CACHE BOOL "Support loading PNGs using libpng" FORCE)
set(SDLIMAGE_AVIF OFF CACHE BOOL "Support loading AVIF images" FORCE)
set(SDLIMAGE_JXL OFF CACHE BOOL "Support loading JXL images" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "Support loading TIFF images" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "Support loading WEBP images" FORCE)
set(SDLIMAGE_SAMPLES OFF CACHE BOOL "Build the SDL3_image sample program(s)" FORCE)
set(SDLIMAGE_INSTALL OFF CACHE BOOL "Enable SDL3_image install target" FORCE)

add_subdirectory(SDL_image)

# nlohmann/json
set(JSON_BuildTests OFF CACHE BOOL "Build the unit tests" FORCE)
set(JSON_Install OFF CACHE BOOL "Install CMake targets during install step" FORCE)

add_subdirectory(json)

# GLM
add_subdirectory(glm)

# Dear ImGui (core + SDL3 platform and SDL_Renderer backends)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui SYSTEM PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC SDL3::SDL3)

# Catch2 - For Testing
if(EXCALCORE_UNIT_TESTS)
  add_subdirectory(Catch2)
endif()
