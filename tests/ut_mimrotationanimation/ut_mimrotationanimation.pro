include(../common_top.pri)

LIBS += $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so -lXfixes

HEADERS += \
    ut_mimrotationanimation.h \
    $$SRC_DIR/mimrotationanimation.h \

SOURCES += \
    ut_mimrotationanimation.cpp \

QT += testlib

include(../common_check.pri)
