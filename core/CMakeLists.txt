set(CORE_LIB_NAME ${PROJECT_NAME}-core)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(rabbitmq-c QUIET)
if (NOT rabbitmq-c_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            rabbitmq-c
            GIT_REPOSITORY "https://github.com/alanxz/rabbitmq-c.git"
            GIT_TAG master
    )
    FetchContent_MakeAvailable(rabbitmq-c)
endif ()

find_package(SimpleAmqpClient QUIET)
if (NOT SimpleAmqpClient_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            SimpleAmqpClient
            GIT_REPOSITORY "https://github.com/alanxz/SimpleAmqpClient.git"
            GIT_TAG master
    )
    FetchContent_MakeAvailable(SimpleAmqpClient)
endif ()

find_package(nstd QUIET)
if (NOT nstd_FOUND)
    include(FetchContent)
    set(nstd_VERSION_TAG "main" CACHE STRING "Version tag of the nstd dependency")
    set(NSTD_ENABLE_TESTING OFF)
    FetchContent_Declare(
            nstd
            GIT_REPOSITORY "https://github.com/mkmagic/nstd"
            GIT_TAG ${nstd_VERSION_TAG}
    )
    FetchContent_MakeAvailable(nstd)

    set(NSTD_LINK_TARGET nstd)
else()
    set(NSTD_LINK_TARGET nstd::nstd)
endif ()


# Adding core library
file(GLOB_RECURSE CORE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
                            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cu")

add_library(${CORE_LIB_NAME} ${CORE_SRCS})
target_include_directories(${CORE_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${CORE_LIB_NAME} PUBLIC SimpleAmqpClient ${NSTD_LINK_TARGET} ${DAWGLOGGER_LINK_TARGET})

# Setting up tests
option(PIPELINE_TOOLS_ENABLE_TESTS "Enable building unit‑tests for ${CORE_LIB_NAME}" OFF)
if(PIPELINE_TOOLS_ENABLE_TESTS)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()