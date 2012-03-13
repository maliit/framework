include(../common_top.pri)

QT += $$QT_WIDGETS

INCLUDEPATH +=  \
    ../stubs

# Input
HEADERS += \
    ft_mimpluginmanager.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/fakeproperty.h \
    ../stubs/mkeyboardstatetracker_stub.h \

SOURCES += \
    ft_mimpluginmanager.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/fakeproperty.cpp \

include($$TOP_DIR/src/libmaliit-plugins.pri)

# For MImInputContextConnection pulled in by TestInputMethodHost
include($$TOP_DIR/connection/libmaliit-connection.pri)

include(../dummyimplugins.pri)

include(../common_check.pri)
