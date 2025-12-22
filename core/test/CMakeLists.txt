set(CORE_LIB_NAME ${PROJECT_NAME}-core)

# ----------------------------------- ################ -----------------------------------
# ----------------------------------- ## GOOGLETEST ## -----------------------------------
# ----------------------------------- ################ -----------------------------------

include(GoogleTest)

find_package(GTest QUIET)
if (NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY "https://github.com/google/googletest.git"
            GIT_TAG main
    )

    # Prevent overriding the parent projects's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

file(GLOB_RECURSE GTEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*_util.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/main_gtest.cpp")

add_executable(${CORE_LIB_NAME}-gtest ${GTEST_SOURCES})
target_link_libraries(${CORE_LIB_NAME}-gtest PRIVATE ${CORE_LIB_NAME} gtest)
gtest_discover_tests(${CORE_LIB_NAME}-gtest)

# ----------------------------------- ############### -----------------------------------
# ----------------------------------- ## BENCHMARK ## -----------------------------------
# ----------------------------------- ############### -----------------------------------

find_package(benchmark QUIET)
if (NOT benchmark_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            benchmark
            GIT_REPOSITORY "https://github.com/google/benchmark.git"
            GIT_TAG main
    )

    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(benchmark)
endif ()

file(GLOB_RECURSE BM_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*_util.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/BM_*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/main_BM.cpp")

add_executable(${CORE_LIB_NAME}-benchmark ${BM_SOURCES})
target_link_libraries(${CORE_LIB_NAME}-benchmark PRIVATE ${CORE_LIB_NAME} benchmark)
target_compile_features(${CORE_LIB_NAME}-benchmark PRIVATE cxx_std_20)