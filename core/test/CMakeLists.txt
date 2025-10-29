# ------------------------------------- GTEST -------------------------------------
# Check if gtest is already installed
find_package(GTest QUIET)
if (NOT GTest_FOUND)
    include(FetchContent)
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
        "${CMAKE_CURRENT_SOURCE_DIR}/*_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/main_gtest.cpp"
)

add_executable(${CORE_LIB_NAME}-gtest ${GTEST_SOURCES})
target_link_libraries(${CORE_LIB_NAME}-gtest PRIVATE gtest gtest_main pthread ${CORE_LIB_NAME})

# ----------------------------------- BENCHMARK -----------------------------------
find_package(benchmark QUIET)
if (NOT benchmark_FOUND)
    set(BENCHMARK_ENABLE_TESTING OFF)
    FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG v1.9.4 # latest stable as of 2025
    )
    FetchContent_MakeAvailable(benchmark)
endif ()

file(GLOB_RECURSE BM_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/BM_*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/main_BM.cpp"
)

add_executable(${CORE_LIB_NAME}-benchmark ${BM_SOURCES})
target_link_libraries(${CORE_LIB_NAME}-benchmark PRIVATE benchmark pthread ${CORE_LIB_NAME})
