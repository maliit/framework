include(../common_top.pri)

QT += $$QT_WIDGETS

INCLUDEPATH += ../../input-context/input-context-plugin
LIBS += -L../../input-context/maliit-input-context -L../../input-context/input-context-plugin -l$$MALIIT_INPUTCONTEXT_TARGETNAME -l$$MALIIT_INPUTCONTEXTPLUGIN_TARGETNAME

LIBS += ../../maliit/lib$${MALIIT_LIB}.so

include($$TOP_DIR/connection/libmaliit-connection.pri)

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

HEADERS += \
    ut_minputcontextplugin.h

SOURCES += \
    ut_minputcontextplugin.cpp

CONFIG += direct-connection

direct-connection {
    LIBS += ../../src/lib$${MALIIT_PLUGINS_LIB}.so
    INCLUDEPATH += ../../src
}


CONFIG += link_pkgconfig

include(../common_check.pri)
