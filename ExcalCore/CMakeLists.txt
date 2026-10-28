project(
  ExcalCore
  VERSION 0.01
  DESCRIPTION "Core Library of ExcalEngine"
)

file(GLOB_RECURSE SRC_CORE CONFIGURE_DEPENDS src/*.cpp)

add_library(ExcalCore STATIC ${SRC_CORE})

target_include_directories(ExcalCore PUBLIC include)
target_link_libraries(ExcalCore PUBLIC
  SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image
  nlohmann_json::nlohmann_json glm::glm
)
