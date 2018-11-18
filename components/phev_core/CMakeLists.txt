cmake_minimum_required(VERSION 3.13)
set(COMPONENT_SRCDIRS "./src")
set(COMPONENT_ADD_INCLUDEDIRS "./src/include")

set(COMPONENT_REQUIRES phev_ota msg_core json)
register_component()