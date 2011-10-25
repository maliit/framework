include(../common_top.pri)

INCLUDEPATH += ../../src \
               ../stubs \

# Input
HEADERS += \
    ut_mimapplication.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \

SOURCES += \
    ut_mimapplication.cpp \
    ../stubs/fakegconf.cpp \

CONFIG += plugin qdbus

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so -lXfixes

include(../common_check.pri)
