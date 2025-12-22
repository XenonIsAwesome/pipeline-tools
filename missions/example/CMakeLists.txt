set(MISSION_NAME "example")
set(MISSION_LIB "mission-${MISSION_NAME}")

file(GLOB_RECURSE MISSION_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(${MISSION_LIB} ${MISSION_SOURCES})

target_include_directories(${MISSION_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/")
target_link_libraries(${MISSION_LIB} pipeline-tools-server pipeline-tools-core)