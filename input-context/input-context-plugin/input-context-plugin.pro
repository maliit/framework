TOP_DIR = ../..

include(../input-context.pri)

TARGET = $$TOP_DIR/lib/plugins/$$MALIIT_INPUTCONTEXTPLUGIN_TARGETNAME

include($$TOP_DIR/input-context/libmaliit-inputcontext-qt4.pri)

# Input
HEADERS += minputcontextplugin.h \
           ../maliit-input-context/minputcontext.h \

SOURCES += minputcontextplugin.cpp \
           ../maliit-input-context/minputcontext.cpp \

CONFIG += plugin

INSTALLS    += target
target.path += $$QT_IM_PLUGIN_PATH
