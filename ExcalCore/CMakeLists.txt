project(
  ExcalCore
  VERSION 0.01
  DESCRIPTION "Core Library of ExcalEngine"
)

file(GLOB_RECURSE SRC_CORE CONFIGURE_DEPENDS src/*.cpp)

add_library(ExcalCore STATIC ${SRC_CORE})

target_include_directories(ExcalCore PUBLIC include)
target_compile_options(ExcalCore PRIVATE -Wall -Wextra -Wno-unused-parameter)
target_link_libraries(ExcalCore PUBLIC
  SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image
  nlohmann_json::nlohmann_json glm::glm imgui
)

# Documentation: build the ExcalCoreDocs target, output goes to docs/html
find_program(DOT_EXECUTABLE dot)
if(DOT_EXECUTABLE)
  set(DOXYGEN_HAVE_DOT YES)
else()
  set(DOXYGEN_HAVE_DOT NO)
endif()

configure_file(docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

add_custom_target(ExcalCoreDocs
  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generating ExcalCore documentation"
  VERBATIM
)
add_dependencies(ExcalCoreDocs doxygen_build)
