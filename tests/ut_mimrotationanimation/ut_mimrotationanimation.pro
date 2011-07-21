include(../common_top.pri)

LIBS += $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so -lXfixes -lX11

HEADERS += \
    ut_mimrotationanimation.h \
    $$SRC_DIR/mimrotationanimation.h \

SOURCES += \
    ut_mimrotationanimation.cpp \

CONFIG += meegotouch
QT += testlib

isEqual(code_coverage_option, off) {
    SOURCES += \
        $$SRC_DIR/mimrotationanimation.cpp
}

include(../common_check.pri)
