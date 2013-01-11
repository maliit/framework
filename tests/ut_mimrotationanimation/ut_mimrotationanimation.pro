include(../common_top.pri)

QT += $$QT_WIDGETS

include(../../src/libmaliit-plugins.pri)

LIBS += -lXfixes

HEADERS += \
    ut_mimrotationanimation.h \
    $$SRC_DIR/mimrotationanimation.h \
    ../utils/gui-utils.h \

SOURCES += \
    ut_mimrotationanimation.cpp \
    ../utils/gui-utils.cpp \

needs_x = yes

include(../common_check.pri)
