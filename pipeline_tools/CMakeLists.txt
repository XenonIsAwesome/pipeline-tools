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

find_package(rabbitmq-c QUIET)
if (NOT rabbitmq-c_FOUND)
    option(RABBITMQ_C_TAG "Tag of the rabbitmq-c library" "master")
    include(FetchContent)
    FetchContent_Declare(
            rabbitmq-c
            GIT_REPOSITORY https://github.com/alanxz/rabbitmq-c.git
            GIT_TAG ${RABBITMQ_C_TAG}
    )
    FetchContent_MakeAvailable(rabbitmq-c)
endif()

find_package(SimpleAmqpClient QUIET)
if (NOT SimpleAmqpClient_FOUND)
    option(SIMPLEAMQPCLIENT_TAG "Tag of the SimpleAmqpClient library" "master")
    include(FetchContent)
    FetchContent_Declare(
            SimpleAmqpClient
            GIT_REPOSITORY https://github.com/alanxz/SimpleAmqpClient.git
            GIT_TAG ${SIMPLEAMQPCLIENT_TAG}
    )
    FetchContent_MakeAvailable(SimpleAmqpClient)
endif()

# Recursively glob all .cpp files in src/
file(GLOB_RECURSE CORE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create the library
add_library(${CORE_LIB_NAME} ${CORE_SOURCES})
target_include_directories(${CORE_LIB_NAME}
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(${CORE_LIB_NAME} INTERFACE nlohmann_json::nlohmann_json SimpleAmqpClient)


set(PT_CORE_ENABLE_TESTING "Enable testing for ${CORE_LIB_NAME}" ON)
if (PT_CORE_ENABLE_TESTING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif ()
