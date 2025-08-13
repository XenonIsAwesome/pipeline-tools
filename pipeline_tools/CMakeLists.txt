
# Recursively glob all .cpp files in src/
file(GLOB_RECURSE PIPELINE_TOOLS_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create the library
add_library(libpt ${PIPELINE_TOOLS_SOURCES})

# Add the src/ folder to include paths
target_include_directories(libpt
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)