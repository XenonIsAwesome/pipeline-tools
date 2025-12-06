# - Find SimpleAmqpClient
# This module finds the SimpleAmqpClient library and headers.
#
# It defines the following variables:
#   SimpleAmqpClient_FOUND          - TRUE if the library was found
#   SimpleAmqpClient_INCLUDE_DIRS   - include directories
#   SimpleAmqpClient_LIBRARIES      - libraries to link
#
# It also defines an imported target:
#   SimpleAmqpClient::SimpleAmqpClient
#
# Usage:
#   find_package(SimpleAmqpClient REQUIRED)
#   target_link_libraries(your_target PRIVATE SimpleAmqpClient::SimpleAmqpClient)
#
# You can optionally set the following variables before calling find_package:
#   SimpleAmqpClient_ROOT           - path to root directory of the library install
#   SimpleAmqpClient_INCLUDE_DIR    - manually specify include directory
#   SimpleAmqpClient_LIBRARY        - manually specify library file

# Support user-defined root path
set(_SimpleAmqpClient_HINTS "")
if(DEFINED SimpleAmqpClient_ROOT)
    list(APPEND _SimpleAmqpClient_HINTS
            "${SimpleAmqpClient_ROOT}"
            "${SimpleAmqpClient_ROOT}/lib"
            "${SimpleAmqpClient_ROOT}/lib64"
            "${SimpleAmqpClient_ROOT}/include"
    )
endif()

# Locate header
find_path(SimpleAmqpClient_INCLUDE_DIR
        NAMES SimpleAmqpClient/SimpleAmqpClient.h
        HINTS
        ${_SimpleAmqpClient_HINTS}
        PATH_SUFFIXES
        include
        SimpleAmqpClient
)

# Locate library
find_library(SimpleAmqpClient_LIBRARY
        NAMES SimpleAmqpClient
        HINTS
        ${_SimpleAmqpClient_HINTS}
        PATH_SUFFIXES
        lib
        lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SimpleAmqpClient
        REQUIRED_VARS
        SimpleAmqpClient_LIBRARY
        SimpleAmqpClient_INCLUDE_DIR
        FAIL_MESSAGE
        "Could not find SimpleAmqpClient library or headers"
)

if(SimpleAmqpClient_FOUND)
    set(SimpleAmqpClient_LIBRARIES ${SimpleAmqpClient_LIBRARY})
    set(SimpleAmqpClient_INCLUDE_DIRS ${SimpleAmqpClient_INCLUDE_DIR})

    if(NOT TARGET SimpleAmqpClient::SimpleAmqpClient)
        add_library(SimpleAmqpClient::SimpleAmqpClient UNKNOWN IMPORTED)
        set_target_properties(SimpleAmqpClient::SimpleAmqpClient PROPERTIES
                IMPORTED_LOCATION "${SimpleAmqpClient_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${SimpleAmqpClient_INCLUDE_DIR}"
        )
    endif()
endif()
