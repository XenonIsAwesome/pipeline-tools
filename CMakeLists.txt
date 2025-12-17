cmake_minimum_required(VERSION 3.20)
project(pipeline-tools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# ------------------------------ ######################### ------------------------------
# ------------------------------ ## Common dependencies ## ------------------------------
# ------------------------------ ######################### ------------------------------
find_package(fmt REQUIRED)

find_package(nlohmann_json QUIET)
if (NOT nlohmann_json_FOUND)
    FetchContent_Declare(
            json
            GIT_REPOSITORY "https://github.com/nlohmann/json.git"
            GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(json)
endif ()

find_package(dawg-logger QUIET)
if (NOT dawg-logger_FOUND)
    set(DAWGLOGGER_VERSION_TAG "main" CACHE STRING "DawgLogger version tag")
    include(FetchContent)
    FetchContent_Declare(
            dawg-logger
            GIT_REPOSITORY "https://github.com/eyaljoseph/dawg-logger.git"
            GIT_TAG ${DAWGLOGGER_VERSION_TAG}
    )
    FetchContent_MakeAvailable(dawg-logger)
    set(DAWGLOGGER_LINK_TARGET dawg-logger)
else()
    set(DAWGLOGGER_LINK_TARGET DawgLog::dawg-logger)
endif ()

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
        mission-example

        -Wl,--no-whole-archive
        pipeline-tools-server pipeline-tools-core)
