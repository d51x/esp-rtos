set(COMPONENT_SRCS "httpd.c" "http_page.c" "http_handlers.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "lib/include")
set(COMPONENT_REQUIRES "freertos")

register_component()
