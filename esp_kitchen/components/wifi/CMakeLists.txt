set(COMPONENT_SRCS "wifi.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "lib/include")
set(COMPONENT_REQUIRES "nvsparam" "freertos")

register_component()
