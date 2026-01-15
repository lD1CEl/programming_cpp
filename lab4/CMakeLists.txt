cmake_minimum_required(VERSION 3.14)
project(flat_hash_map_lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -msse2)
endif()

add_library(FlatHashMap INTERFACE)
target_include_directories(FlatHashMap INTERFACE ${CMAKE_SOURCE_DIR}/include)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(flat_hash_map_tests tests/flat_hash_map_tests.cpp)
target_link_libraries(flat_hash_map_tests PRIVATE FlatHashMap GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(flat_hash_map_tests)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE FlatHashMap)
