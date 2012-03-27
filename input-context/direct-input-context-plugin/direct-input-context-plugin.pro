include(../input-context.pri)

TARGET = $$MALIIT_DIRECTINPUTCONTEXTPLUGIN_TARGETNAME

INCLUDEPATH += ../maliit-input-context
LIBS += ../maliit-input-context/lib$${MALIIT_INPUTCONTEXT_TARGETNAME}.a

LIBS += ../../src/lib$${MALIIT_PLUGINS_LIB}.so
INCLUDEPATH += ../../src

# Input
HEADERS += mdirectinputcontextplugin.h \

SOURCES += mdirectinputcontextplugin.cpp \

CONFIG += plugin

INSTALLS    += target \
target.path += $$QT_IM_PLUGIN_PATH
