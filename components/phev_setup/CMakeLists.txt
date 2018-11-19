set(COMPONENT_SRCDIRS "./src")
set(COMPONENT_ADD_INCLUDEDIRS "./src/include")

set(COMPONENT_REQUIRES nvs_flash esp_http_server phev_core wifi_client ppp_client)

set(COMPONENT_EMBED_TXTFILES "./resources/phev-remote-config.html")

register_component()