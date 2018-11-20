
set(COMPONENT_SRCDIRS "./src")

set(COMPONENT_REQUIRES wifi_client nvs_flash mdns phev_app msg_core phev_setup phev_store)

register_component()