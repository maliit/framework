include(../common_top.pri)

LIBS += $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so -lXfixes

# Input
HEADERS += \
    ut_passthroughserver.h \

SOURCES += \
    ut_passthroughserver.cpp \

QT += testlib

include(../common_check.pri)
