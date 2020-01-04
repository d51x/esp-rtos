#
# Main Makefile. This is basically the same as a component makefile.
#
COMPONENT_SRCDIRS := 	. \
					 	core \
						core/ota

COMPONENT_EMBED_FILES := 	${PROJECT_PATH}/web/favicon.ico \
							${PROJECT_PATH}/web/main.css \
							${PROJECT_PATH}/web/ajax.js