project(
  ExcalSandbox
  VERSION 0.01
  DESCRIPTION "A testing playground for the Core and Runtime of ExcalEngine"
)

file(GLOB_RECURSE SRC_SANDBOX CONFIGURE_DEPENDS src/*.cpp)

add_executable(ExcalSandbox ${SRC_SANDBOX})

target_include_directories(ExcalSandbox PRIVATE include src)
target_link_libraries(ExcalSandbox PRIVATE ExcalCore)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
  excal_add_assets(ExcalSandbox ${CMAKE_CURRENT_SOURCE_DIR}/assets)
endif()

# No console window behind the game in release builds on Windows
if(WIN32)
  set_target_properties(ExcalSandbox PROPERTIES
    WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
  )
endif()
