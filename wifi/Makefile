#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
PROJECT_NAME := wifi

            #ifneq ("$(wildcard ${PROJECT_PATH}/version.txt)","")
			ifneq ("$(wildcard version.txt)","")
                #PROJECT_VER := $(shell cat ${PROJECT_PATH}/version.txt)
                PROJECT_VER := $(shell cat version.txt)
            endif         
            # else
                 GIT_PROJECT_VER := $(shell cd ../${PROJECT_PATH} && git describe --always --tags 2> /dev/null)
                 ifeq ("${GIT_PROJECT_VER}", "")
            #         PROJECT_VER := "1"
                     #$(info Project is not inside a git repository, or git repository has no commits)
                     #$(info will not use 'git describe' to determine PROJECT_VER.)
                 else
                     PROJECT_VER:= $(PROJECT_VER)_$(GIT_PROJECT_VER)
                 endif # a git repository
            # endif # version.txt

#PROJECT_NAME := $(PROJECT_NAME)_$(PROJECT_VER)
include $(IDF_PATH)/make/project.mk



