cmake_minimum_required(VERSION 3.14)

# Define the test target
project(pipeline_tools_tests LANGUAGES CXX)

enable_testing()

# --- GoogleTest setup ---
include(FetchContent)

# Check if gtest is already installed
find_package(GTest QUIET)

if (NOT GTest_FOUND)
    message(STATUS "GTest not found, fetching with FetchContent...")

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.15.2 # latest stable as of 2025
    )

    # Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# --- Collect all test sources ---
file(GLOB_RECURSE TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# --- Create test executable ---
add_executable(pipeline_tools_tests ${TEST_SOURCES})

target_include_directories(pipeline_tools_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/pipeline_tools/src
)

target_link_libraries(pipeline_tools_tests
        PRIVATE
        libpt
        GTest::gtest
        GTest::gtest_main
        pthread   # needed on Linux
)

# --- Register with CTest ---
include(GoogleTest)
gtest_discover_tests(pipeline_tools_tests)
