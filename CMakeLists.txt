cmake_minimum_required(VERSION 3.20)
project(pipeline-tools LANGUAGES CXX)

include(cmake/kconfig.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)
if(ENABLE_COVERAGE)
    include(CTest)

    message(STATUS "Build with coverage instrumentation")
    add_compile_options(--coverage -O0)
    add_link_options(--coverage)
endif()

# ------------------------------ ########################## ------------------------------
# ------------------------------ ## Setting up libraries ## ------------------------------
# ------------------------------ ########################## ------------------------------
add_subdirectory(core)
add_subdirectory(server)
add_subdirectory(missions)

# ------------------------------ ##################### ------------------------------
# ------------------------------ ## Main executable ## ------------------------------
# ------------------------------ ##################### ------------------------------
add_executable(${PROJECT_NAME} main.cpp)

# TODO: make this automatically populate by adding sub-directories
target_link_libraries(${PROJECT_NAME} PRIVATE
        -Wl,--whole-archive
        example-missions

        -Wl,--no-whole-archive
        pipeline-tools-server pipeline-tools-core)
