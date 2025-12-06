cmake_minimum_required(VERSION 3.14)
project(pt)
set(CMAKE_CXX_STANDARD 20)

set(MAIN_EXEC_NAME ${PROJECT_NAME}-example)
set(CORE_LIB_NAME ${PROJECT_NAME}-core)

# Library setup
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/pipeline_tools")

# Main executable
add_executable(${MAIN_EXEC_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
target_link_libraries(${MAIN_EXEC_NAME} ${CORE_LIB_NAME})
