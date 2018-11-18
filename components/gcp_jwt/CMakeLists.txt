
set(COMPONENT_SRCDIRS "./src")
set(COMPONENT_ADD_INCLUDEDIRS "./src/include")
set(COMPONENT_EMBED_TXTFILES "../../main/resources/rsa_private2.pem")

set(COMPONENT_REQUIRES libjwt logger)

register_component()