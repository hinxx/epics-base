#*************************************************************************
# EPICS BASE is distributed subject to a Software License Agreement found
# in file LICENSE that is included with this distribution.
#*************************************************************************

TOP = ..
include $(TOP)/configure/CONFIG
include CONFIG_SITE.local

DIRS += libcom

DIRS += ca
ca_DEPEND_DIRS = libcom

DIRS += database
database_DEPEND_DIRS = ca

# Allow sites to add extra submodules
-include Makefile.local

include $(TOP)/configure/RULES_MODULES
