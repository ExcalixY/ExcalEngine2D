project(
  ExcalStressBehavior
  VERSION 0.01
  DESCRIPTION "Stress test: bouncing tokens updated through Behaviors"
)

file(GLOB_RECURSE SRC_STRESS_BEHAVIOR CONFIGURE_DEPENDS src/*.cpp)

add_executable(ExcalStressBehavior ${SRC_STRESS_BEHAVIOR})

target_include_directories(ExcalStressBehavior PRIVATE include src)
target_link_libraries(ExcalStressBehavior PRIVATE ExcalCore)

excal_add_assets(ExcalStressBehavior ${CMAKE_SOURCE_DIR}/ExcalSandbox/assets)
