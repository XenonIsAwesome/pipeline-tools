set(CORE_LIB_NAME ${PROJECT_NAME}-core)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(SimpleAmqpClient CONFIG REQUIRED)
find_package(nstd CONFIG REQUIRED)
find_package(dawg-logger CONFIG REQUIRED)

# Adding core library
file(GLOB_RECURSE CORE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
                            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cu")

add_library(${CORE_LIB_NAME} ${CORE_SRCS})
target_include_directories(${CORE_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${CORE_LIB_NAME} PUBLIC SimpleAmqpClient::SimpleAmqpClient nstd::nstd DawgLog::dawg-logger)

# Setting up tests
option(PIPELINE_TOOLS_ENABLE_TESTS "Enable building unit‑tests for ${CORE_LIB_NAME}" OFF)
if(PIPELINE_TOOLS_ENABLE_TESTS)
    enable_testing()
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()