# Recursively glob all .cpp files in src/
file(GLOB_RECURSE CORE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Find nlohmann_json library for Node-RED integration
find_package(nlohmann_json 3.2.0 QUIET)
if (NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, attempting to fetch from GitHub...")
    include(FetchContent)
    FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
    FetchContent_MakeAvailable(json)
endif()

# Create the library
add_library(${CORE_LIB_NAME} ${CORE_SOURCES})

# Link nlohmann_json
target_link_libraries(${CORE_LIB_NAME} PUBLIC nlohmann_json::nlohmann_json)

# Add the src/ folder to include paths
target_include_directories(${CORE_LIB_NAME}
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set(PT_CORE_ENABLE_TESTING "Enable testing for ${CORE_LIB_NAME}" ON)
if (PT_CORE_ENABLE_TESTING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif ()
