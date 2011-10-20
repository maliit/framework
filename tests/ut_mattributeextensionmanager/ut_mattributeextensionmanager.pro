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

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \

target.files += \
    $$TARGET \
    toolbar1.xml \
    toolbar2.xml \

include(../common_check.pri)
