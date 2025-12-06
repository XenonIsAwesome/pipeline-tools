cmake_minimum_required(VERSION 3.14)
project(pt)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/pipeline_tools")

add_executable(${PROJECT_NAME}-example "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
target_link_libraries(${PROJECT_NAME} pt-core)
