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
            GIT_TAG v1.15.2 # latest stable as of 2025
    )

    # Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif ()

# ----------------- GTest target -----------------
file(GLOB_RECURSE GTEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/testing_utils/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/main_gtest.cpp"
)

add_executable(${CORE_LIB_NAME}-gtest ${GTEST_SOURCES})
target_link_libraries(${CORE_LIB_NAME}-gtest PRIVATE gtest gtest_main pthread libpt)
# assuming "libpt" is your main library

add_test(NAME ${CORE_LIB_NAME}-gtest COMMAND ${CORE_LIB_NAME}-gtest)

# Check if gtest is already installed
find_package(benchmark QUIET)
if (NOT benchmark_FOUND)
    message(STATUS "benchmark not found, fetching with FetchContent...")

    FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG v1.9.4 # latest stable as of 2025
    )
    FetchContent_MakeAvailable(benchmark)
endif ()

# ----------------- Benchmark target -----------------
file(GLOB_RECURSE BM_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/testing_utils/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/BM_*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/main_BM.cpp"
)

add_executable(${CORE_LIB_NAME}-benchmark ${BM_SOURCES})
target_link_libraries(${CORE_LIB_NAME}-gtest PRIVATE benchmark pthread libpt)
