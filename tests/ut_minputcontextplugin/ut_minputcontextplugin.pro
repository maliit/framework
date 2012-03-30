include(../common_top.pri)

QT += $$QT_WIDGETS

INCLUDEPATH += ../../input-context/input-context-plugin
LIBS += -L../../input-context/input-context-plugin -l$$MALIIT_INPUTCONTEXTPLUGIN_TARGETNAME

include($$TOP_DIR/maliit/libmaliit.pri)
include($$TOP_DIR/common/libmaliit-common.pri)
include($$TOP_DIR/connection/libmaliit-connection.pri)

# For MImSettings
include($$TOP_DIR/src/libmaliit-plugins.pri)

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

HEADERS += \
    ut_minputcontextplugin.h

SOURCES += \
    ut_minputcontextplugin.cpp

CONFIG += link_pkgconfig

include(../common_check.pri)
