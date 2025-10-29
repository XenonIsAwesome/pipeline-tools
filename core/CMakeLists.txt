set(CORE_LIB_NAME ${PROJECT_NAME}-core)

find_package(nlohmann_json QUIET)
if (NOT nlohmann_json_FOUND)
    option(NLOHMANN_JSON_TAG "Tag of the nlohmann_json library" "master")
    include(FetchContent)
    FetchContent_Declare(
            nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG ${RABBITMQ_C_TAG}
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Create the library
file(GLOB_RECURSE CORE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/pt/core/*.cpp"
)
add_library(${CORE_LIB_NAME} ${CORE_SOURCES})
target_include_directories(${CORE_LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(${CORE_LIB_NAME} INTERFACE nlohmann_json)

set(ENABLE_TESTS "Enable testing for libpt" ON)
if (ENABLE_TESTS)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif ()
