project(
  ExcalTestGame
  VERSION 0.01
  DESCRIPTION "Chip Catch: a tiny game exercising Behaviors, input, sprites and destruction"
)

file(GLOB_RECURSE SRC_TEST_GAME CONFIGURE_DEPENDS src/*.cpp)

add_executable(ExcalTestGame ${SRC_TEST_GAME})

target_include_directories(ExcalTestGame PRIVATE src)
target_link_libraries(ExcalTestGame PRIVATE ExcalCore)

excal_add_assets(ExcalTestGame ${CMAKE_SOURCE_DIR}/ExcalSandbox/assets)
