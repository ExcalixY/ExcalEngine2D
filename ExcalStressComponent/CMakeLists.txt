project(
  ExcalStressComponent
  VERSION 0.01
  DESCRIPTION "Stress test: bouncing tokens updated through a component system"
)

file(GLOB_RECURSE SRC_STRESS_COMPONENT CONFIGURE_DEPENDS src/*.cpp)

add_executable(ExcalStressComponent ${SRC_STRESS_COMPONENT})

target_include_directories(ExcalStressComponent PRIVATE include src)
target_link_libraries(ExcalStressComponent PRIVATE ExcalCore)

excal_add_assets(ExcalStressComponent ${CMAKE_SOURCE_DIR}/ExcalSandbox/assets)

# Both stress apps copy the same assets into the same bin/Assets; serialise them
add_dependencies(ExcalStressComponent ExcalStressBehavior)
