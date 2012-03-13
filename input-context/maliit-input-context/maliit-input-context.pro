TOP_DIR = ../..

include(../input-context.pri)

CONFIG += staticlib
TARGET = $$MALIIT_INPUTCONTEXT_TARGETNAME

# Input
HEADERS += minputcontext.h

SOURCES += minputcontext.cpp

target.path += $$M_IM_INSTALL_LIBS
