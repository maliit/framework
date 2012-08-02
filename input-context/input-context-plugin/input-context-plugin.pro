TOP_DIR = ../..

include($$TOP_DIR/config.pri)

TARGET = $$TOP_DIR/lib/plugins/$$MALIIT_INPUTCONTEXTPLUGIN_TARGETNAME

include($$TOP_DIR/input-context/libmaliit-inputcontext-qt4.pri)
include(../input-context.pri)

# Input
HEADERS += minputcontextplugin.h \

SOURCES += minputcontextplugin.cpp \

CONFIG += plugin

INSTALLS    += target
target.path += $$QT_IM_PLUGIN_PATH
