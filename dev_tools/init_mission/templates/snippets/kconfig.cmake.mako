<%
def format_name(name: str):
    mapping = {
        ' ': '_',
        '-': '_',
    }

    for k, v in mapping.items():
        name = name.replace(k, v)

    return name
%>
# ${mission_name}
if(KCONFIG_RAW MATCHES "\nCONFIG_ENABLE_${format_name(mission_name).upper()}_MISSIONS=y\n")
    set(ENABLE_${format_name(mission_name).upper()}_MISSIONS ON)
endif()

if(KCONFIG_RAW MATCHES "\nCONFIG_ENABLE_${format_name(mission_name).upper()}_MISSIONS=n\n")
    set(ENABLE_${format_name(mission_name).upper()}_MISSIONS OFF)
endif()
