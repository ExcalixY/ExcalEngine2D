file(GLOB_RECURSE SRC_TESTS *.cpp)

add_executable(ExcalTests ${SRC_TESTS})
target_link_libraries(ExcalTests  PRIVATE ExcalCore Catch2::Catch2WithMain)

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(ExcalTests)
