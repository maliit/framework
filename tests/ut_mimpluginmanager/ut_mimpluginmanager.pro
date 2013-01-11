include(../common_top.pri)

QT += $$QT_WIDGETS

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mimpluginmanager.h \

SOURCES += \
    ut_mimpluginmanager.cpp \

include(../dummyimplugins.pri)
include($$TOP_DIR/src/libmaliit-plugins.pri)

# For MImInputContextConnection pulled in by TestInputMethodHost
include($$TOP_DIR/connection/libmaliit-connection.pri)

target.files += \
    $$TARGET \
    toolbar1.xml \
    toolbar2.xml \

needs_x = yes

include(../common_check.pri)
