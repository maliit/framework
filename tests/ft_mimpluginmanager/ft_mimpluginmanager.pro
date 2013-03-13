include(../common_top.pri)

QT += gui

INCLUDEPATH +=  \
    ../stubs

# Input
HEADERS += \
    ft_mimpluginmanager.h \
    ../stubs/mkeyboardstatetracker_stub.h \
    ../stubs/fakeproperty.h \

SOURCES += \
    ft_mimpluginmanager.cpp \
    ../stubs/fakeproperty.cpp \

include($$TOP_DIR/src/libmaliit-plugins.pri)

# For MImInputContextConnection pulled in by TestInputMethodHost
include($$TOP_DIR/connection/libmaliit-connection.pri)

include(../dummyimplugins.pri)

include(../common_check.pri)
