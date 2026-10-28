cmake_minimum_required(VERSION 4.0)

project(ExcalEngine
  VERSION 0.01
  LANGUAGES CXX C
  DESCRIPTION "A remake of ExcalEngine to work with 2D games."
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OUT_DIR ${CMAKE_SOURCE_DIR}/out)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${OUT_DIR}/${CMAKE_SYSTEM_NAME}/bin/
)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${OUT_DIR}/${CMAKE_SYSTEM_NAME}/lib/
)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${OUT_DIR}/${CMAKE_SYSTEM_NAME}/archive/
)

# Bundle the GCC runtime and winpthread so the exe runs without extra DLLs
if(MINGW)
  add_link_options(-static -static-libgcc -static-libstdc++)
  add_link_options($<$<CONFIG:Release>:-s>)
endif()

add_subdirectory(libs)

add_subdirectory(ExcalCore)
add_subdirectory(ExcalSandbox)
