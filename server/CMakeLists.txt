set(SERVER_LIB_NAME ${PROJECT_NAME}-server)

# Adding library
file(GLOB_RECURSE SERVER_LIB_SOURCES src/server/*.cpp)
add_library(${SERVER_LIB_NAME} STATIC ${SERVER_LIB_SOURCES})
target_include_directories(${SERVER_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(httplib QUIET)
if (NOT httplib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            httplib
            GIT_REPOSITORY "https://github.com/yhirose/cpp-httplib"
            GIT_TAG v0.26.0
    )
    FetchContent_MakeAvailable(httplib)
endif()

target_link_libraries(${SERVER_LIB_NAME} PUBLIC nlohmann_json::nlohmann_json httplib::httplib pipeline-tools-core ${DAWGLOGGER_LINK_TARGET})

set(SWAGGER_JSON_PATH "${CMAKE_BINARY_DIR}/swagger.json")
target_compile_definitions(${SERVER_LIB_NAME} PUBLIC SWAGGER_JSON_PATH="${SWAGGER_JSON_PATH}")
