#
# Main Makefile. This is basically the same as a component makefile.
#
COMPONENT_SRCDIRS := 	. \
					 	core \
					   	core/sensors \
						core/drivers \
						core/ota \
						rgb

COMPONENT_EMBED_FILES := 	${PROJECT_PATH}/main/web/favicon.ico \
							${PROJECT_PATH}/main/web/main.css 