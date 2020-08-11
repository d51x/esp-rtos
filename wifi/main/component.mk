#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the 
# src/ directory, compile them and link them into lib(subdirectory_name).a 
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#
COMPONENT_SRCDIRS := 	. \
					 	core


COMPONENT_EMBED_FILES := 	${PROJECT_PATH}/web/favicon.ico \
							${PROJECT_PATH}/web/main.min.css \
							${PROJECT_PATH}/web/ajax.min.js	\
							${PROJECT_PATH}/web/menu.png	\
							${PROJECT_PATH}/web/menu2.png
