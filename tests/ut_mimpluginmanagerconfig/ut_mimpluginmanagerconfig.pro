include(../common_top.pri)

QT += gui

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mimpluginmanagerconfig.h \

SOURCES += \
    ut_mimpluginmanagerconfig.cpp \

include(../dummyimplugins.pri)
include($$TOP_DIR/src/libmaliit-plugins.pri)

# For MImInputContextConnection pulled in by TestInputMethodHost
include($$TOP_DIR/connection/libmaliit-connection.pri)

target.files += \
    $$TARGET \

include(../common_check.pri)
