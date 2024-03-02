##########################################################################
# Copyright (c) 2011 UChicago Argonne LLC, as Operator of Argonne
#     National Laboratory.
# EPICS BASE is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution.
##########################################################################

# This is a Makefile fragment, see src/ioc/Makefile.

SRC_DIRS += $(STDDIR)/imSoftIoc

PROD_IOC_DEFAULT = imSoftIoc
PROD_IOC_iOS = -nil-

DBD += base.dbd
DBD += asSub.dbd
DBD += imSoftIoc.dbd

imSoftIoc_DBD += base.dbd
imSoftIoc_DBD += dlload.dbd
imSoftIoc_DBD += system.dbd

imSoftIoc_SRCS += imSoftIoc_registerRecordDeviceDriver.cpp
imSoftIoc_SRCS_DEFAULT += imSoftMain.cpp
imSoftIoc_SRCS_vxWorks = -nil-

imSoftIoc_LIBS = $(EPICS_BASE_IOC_LIBS)

DB += imSoftIocExit.db

CLEANS += epicsInstallDir.h

# imgui stuff
imSoftIoc_SRCS += main.cpp
imSoftIoc_SRCS += imgui.cpp
imSoftIoc_SRCS += imgui_demo.cpp
imSoftIoc_SRCS += imgui_draw.cpp
imSoftIoc_SRCS += imgui_tables.cpp
imSoftIoc_SRCS += imgui_widgets.cpp
imSoftIoc_SRCS += imgui_impl_glfw.cpp
imSoftIoc_SRCS += imgui_impl_opengl3.cpp

PROD_SYS_LIBS += GL
# pkg-config --static --libs glfw3
# -lglfw -lrt -lm -ldl
PROD_SYS_LIBS += glfw rt m dl
USR_CXXFLAGS += -std=c++11 -g -Wall -Wformat
# pkg-config --cflags glfw3
# <empty>

