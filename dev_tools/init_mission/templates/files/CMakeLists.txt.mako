<%
def format_name(name: str):
    mapping = {
        ' ': '_',
        '-': '_',
    }

    for k, v in mapping.items():
        name = name.replace(k, v)

    return name

def bool_to_on_off(flag: bool):
    return "ON" if flag else "OFF"
%>
option(ENABLE_${format_name(mission_name).upper()}_MISSIONS "Enables the '${mission_name}' missions library" ${bool_to_on_off(default)})
if (ENABLE_${format_name(mission_name).upper()}_MISSIONS)
    set(MISSION_NAME "${format_name(mission_name).lower()}")
    set(MISSION_LIB "${"${"}MISSION_NAME}-missions")

    file(GLOB_RECURSE MISSION_SOURCES "${"${"}CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
    add_library(${"${"}MISSION_LIB} ${"${"}MISSION_SOURCES})

    target_include_directories(${"${"}MISSION_LIB} PUBLIC "${"${"}CMAKE_CURRENT_SOURCE_DIR}/src/")
    target_link_libraries(${"${"}MISSION_LIB} pipeline-tools-server pipeline-tools-core)
endif()