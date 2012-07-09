TOP_DIR = ../..

include(../input-context.pri)

CONFIG += staticlib
TARGET = $$TOP_DIR/lib/$$MALIIT_INPUTCONTEXT_TARGETNAME

# Input
HEADERS += minputcontext.h

SOURCES += minputcontext.cpp

target.path += $$LIBDIR
