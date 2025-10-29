cmake_minimum_required(VERSION 3.14)
project(pt)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/core")

add_executable(${PROJECT_NAME}-example "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
target_link_libraries(${PROJECT_NAME}-example ${PROJECT_NAME}-core)
