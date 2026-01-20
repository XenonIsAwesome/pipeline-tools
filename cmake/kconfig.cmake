set(KCONFIG_FILE "${CMAKE_SOURCE_DIR}/.config")

if(NOT EXISTS "${KCONFIG_FILE}")
    message(WARNING "No .config found. If you want to use the menu configuration tool, run `kconfig-nconf Kconfig` first.")
    return()
endif()

file(READ "${KCONFIG_FILE}" KCONFIG_RAW)

if(KCONFIG_RAW MATCHES "\nCONFIG_PIPELINE_TOOLS_ENABLE_TESTING=y\n")
    set(PIPELINE_TOOLS_ENABLE_TESTING ON)
endif()

if (KCONFIG_RAW MATCHES "\n_CONFIG_ENABLE_COVERAGE=y\n")
    set(ENABLE_COVERAGE ON)
endif()

# BEGIN AUTO-GENERATED SECTION

if(KCONFIG_RAW MATCHES "\nCONFIG_ENABLE_EXAMPLE_MISSIONS=y\n")
    set(ENABLE_EXAMPLE_MISSIONS ON)
endif()

# END AUTO-GENERATED SECTION