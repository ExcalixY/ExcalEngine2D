cmake_minimum_required(VERSION 4.0)

project(ExcalEngine
  VERSION 0.01
  LANGUAGES CXX C
  DESCRIPTION "A remake of ExcalEngine to work with 2D games."
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

# No C++20 modules are used; scanning only adds GCC-only flags that break clangd/clang-tidy
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(OUT_DIR ${CMAKE_SOURCE_DIR}/out/${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/bin/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUT_DIR}/lib/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUT_DIR}/archive/)

include(cmake/ExcalAssets.cmake)

option(EXCALCORE_UNIT_TESTS "Build ExcalCore unit tests" ON)

# Bundle the GCC runtime and winpthread so the exe runs without extra DLLs
if(MINGW)
  add_link_options(-static -static-libgcc -static-libstdc++)
  add_link_options($<$<CONFIG:Release>:-s>)
endif()

add_subdirectory(libs)

add_subdirectory(ExcalCore)
add_subdirectory(ExcalSandbox)
add_subdirectory(ExcalStressBehavior)
add_subdirectory(ExcalStressComponent)

if(EXCALCORE_UNIT_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
