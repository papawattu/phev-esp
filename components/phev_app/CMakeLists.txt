set(COMPONENT_SRCDIRS "./src")
set(COMPONENT_ADD_INCLUDEDIRS "./src/include")

set(COMPONENT_REQUIRES logger wifi_client phev_core mqtt gcp_jwt tcp_client)

register_component()