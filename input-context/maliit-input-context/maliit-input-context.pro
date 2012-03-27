include(../input-context.pri)

CONFIG += staticlib
TARGET = $$MALIIT_INPUTCONTEXT_TARGETNAME

include($$TOP_DIR/connection/libmaliit-connection.pri)

# Input
HEADERS += minputcontext.h

SOURCES += minputcontext.cpp

target.path += $$M_IM_INSTALL_LIBS
