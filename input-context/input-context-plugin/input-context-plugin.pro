TOP_DIR = ../..

include(../input-context.pri)

TARGET = $$MALIIT_INPUTCONTEXTPLUGIN_TARGETNAME

include($$TOP_DIR/input-context/libmaliit-inputcontext-qt4.pri)

# Input
HEADERS += minputcontextplugin.h \

SOURCES += minputcontextplugin.cpp \

CONFIG += plugin

INSTALLS    += target
target.path += $$QT_IM_PLUGIN_PATH
