find_path(SimpleAmqpClient_INCLUDE_DIRS SimpleAmqpClient/SimpleAmqpClient.h)

find_library(SimpleAmqpClient_LIBRARIES SimpleAmqpClient)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SimpleAmqpClient DEFAULT_MSG
        SimpleAmqpClient_INCLUDE_DIRS SimpleAmqpClient_LIBRARIES)

mark_as_advanced(SimpleAmqpClient_INCLUDE_DIRS SimpleAmqpClient_LIBRARIES)

find_package(Boost ${BOOST_VERSION} COMPONENTS chrono system REQUIRED)

if (SimpleAmqpClient_FOUND AND NOT (TARGET SimpleAmqpClient::SimpleAmqpClient))
    file(READ "${SimpleAmqpClient_INCLUDE_DIRS}/SimpleAmqpClient/Version.h" SAC_VERSION)
    foreach (component MAJOR MINOR PATCH)
        string(REGEX MATCH "#define +SIMPLEAMQPCLIENT_VERSION_${component} +([0-9]+)" _ ${SAC_VERSION})
        set(SimpleAmqpClient_VERSION_${component} "${CMAKE_MATCH_1}")
    endforeach ()
    set(SimpleAmqpClient_VERSION "${SimpleAmqpClient_VERSION_MAJOR}.${SimpleAmqpClient_VERSION_MINOR}.${SimpleAmqpClient_VERSION_PATCH}")
    add_library(SimpleAmqpClient::SimpleAmqpClient UNKNOWN IMPORTED)
    set_target_properties(SimpleAmqpClient::SimpleAmqpClient PROPERTIES
            LINKER_LANGUAGE CXX
            IMPORTED_LOCATION ${SimpleAmqpClient_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES ${SimpleAmqpClient_INCLUDE_DIRS}
            INTERFACE_LINK_LIBRARIES "${Boost_LIBRARIES}"
            VERSION ${SimpleAmqpClient_VERSION})
endif ()
