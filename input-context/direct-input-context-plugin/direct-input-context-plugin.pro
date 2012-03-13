TOP_DIR = ../..

include(../input-context.pri)

TARGET = $$MALIIT_DIRECTINPUTCONTEXTPLUGIN_TARGETNAME

include($$TOP_DIR/input-context/libmaliit-inputcontext-qt4.pri)
include($$TOP_DIR/src/libmaliit-plugins.pri)

# Input
HEADERS += mdirectinputcontextplugin.h \

SOURCES += mdirectinputcontextplugin.cpp \

CONFIG += plugin

INSTALLS    += target
target.path += $$QT_IM_PLUGIN_PATH
