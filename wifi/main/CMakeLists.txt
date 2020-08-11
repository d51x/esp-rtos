# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_TXTFILES 	${PROJECT_PATH}/web/favicon.ico
								${PROJECT_PATH}/web/menu.png
								${PROJECT_PATH}/web/menu2.png
								${PROJECT_PATH}/web/main.min.css
								${PROJECT_PATH}/web/ajax.min.js )

register_component()
