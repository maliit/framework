include(../common_top.pri)

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mkeyoverride.h \

SOURCES += \
    ut_mkeyoverride.cpp \

QT += dbus
CONFIG += plugin

include($$TOP_DIR/src/libmaliit-plugins.pri)
include($$TOP_DIR/connection/libmaliit-connection.pri)

include(../common_check.pri)
