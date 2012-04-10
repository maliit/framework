include(../common_top.pri)

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mkeyoverride.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \

SOURCES += \
    ut_mkeyoverride.cpp \
    ../stubs/fakegconf.cpp \

CONFIG += plugin qdbus

include($$TOP_DIR/src/libmaliit-plugins.pri)
include($$TOP_DIR/connection/libmaliit-connection.pri)

include(../common_check.pri)
