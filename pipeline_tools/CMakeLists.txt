# Recursively glob all .cpp files in src/
file(GLOB_RECURSE CORE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create the library
add_library(${CORE_LIB_NAME} ${CORE_SOURCES})

# Add the src/ folder to include paths
target_include_directories(${CORE_LIB_NAME}
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set(PT_CORE_ENABLE_TESTING "Enable testing for ${CORE_LIB_NAME}" ON)
if (PT_CORE_ENABLE_TESTING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif ()
