include(../common_top.pri)

QT += $$QT_WIDGETS

LIBS += $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so -lXfixes

HEADERS += \
    ut_mimrotationanimation.h \
    $$SRC_DIR/mimrotationanimation.h \
    ../utils/gui-utils.h \

SOURCES += \
    ut_mimrotationanimation.cpp \
    ../utils/gui-utils.cpp \

include(../common_check.pri)
