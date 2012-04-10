include(../common_top.pri)

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mattributeextensionmanager.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \


SOURCES += \
    ut_mattributeextensionmanager.cpp \
    ../stubs/fakegconf.cpp \

CONFIG += plugin qdbus

include($$TOP_DIR/src/libmaliit-plugins.pri)
include($$TOP_DIR/connection/libmaliit-connection.pri)

target.files += \
    $$TARGET \
    toolbar1.xml \
    toolbar2.xml \

include(../common_check.pri)
