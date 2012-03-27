include(../input-context.pri)

TARGET = $$MALIIT_INPUTCONTEXTPLUGIN_TARGETNAME

INCLUDEPATH += ../maliit-input-context
LIBS += ../maliit-input-context/lib$${MALIIT_INPUTCONTEXT_TARGETNAME}.a

# Input
HEADERS += minputcontextplugin.h \

SOURCES += minputcontextplugin.cpp \

CONFIG += plugin

INSTALLS    += target
target.path += $$QT_IM_PLUGIN_PATH
