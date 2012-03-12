include(../common_top.pri)

QT += $$QT_WIDGETS

INCLUDEPATH += ../../input-context
LIBS += -L../../input-context -l$$MALIIT_INPUTCONTEXT_TARGETNAME

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
