<%
def format_name(name: str):
    mapping = {
        ' ': '_',
        '-': '_',
    }

    for k, v in mapping.items():
        name = name.replace(k, v)

    return name

def bool_to_yn(flag: bool):
    return "y" if flag else "n"

%>
config ENABLE_${format_name(mission_name).upper()}_MISSIONS
    bool "Enable the '${mission_name}' missions library"
    default ${bool_to_yn(default)}
