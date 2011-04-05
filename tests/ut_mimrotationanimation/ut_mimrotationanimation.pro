include(../common_top.pri)

LIBS += ../../src/libmeegoimframework.so -lXfixes

HEADERS += \
    ut_mimrotationanimation.h \
    $$SRC_DIR/mimrotatinanimation.h \

SOURCES += \
    ut_mimrotationanimation.cpp \

CONFIG += meegotouch
QT += testlib

isEqual(code_coverage_option, off) {
    SOURCES += \
        $$SRC_DIR/mimrotationanimation.cpp
}else {
    QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
    # requires to run make (for $$SRC_DIR) before make check:
    LIBS += $$SRC_DIR/mimrotationanimation.o
}

include(../common_check.pri)
