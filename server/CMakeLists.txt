set(SERVER_LIB_NAME ${PROJECT_NAME}-server)

# Adding library
file(GLOB_RECURSE SERVER_LIB_SOURCES src/server/*.cpp)
add_library(${SERVER_LIB_NAME} STATIC ${SERVER_LIB_SOURCES})
target_include_directories(${SERVER_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(dawg-logger CONFIG REQUIRED)

target_link_libraries(${SERVER_LIB_NAME} PUBLIC nlohmann_json::nlohmann_json httplib::httplib pipeline-tools-core DawgLog::dawg-logger)

set(SWAGGER_JSON_PATH "${CMAKE_BINARY_DIR}/swagger.json")
target_compile_definitions(${SERVER_LIB_NAME} PUBLIC SWAGGER_JSON_PATH="${SWAGGER_JSON_PATH}")
