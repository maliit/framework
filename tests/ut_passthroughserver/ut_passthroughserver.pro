include(../common_top.pri)

SRC_PATH += ../../passthroughserver
INCLUDEPATH += $$SRC_PATH

LIBS += ../../src/libmeegoimframework.so -lXfixes

# Input
HEADERS += \
    ut_passthroughserver.h \
    $$SRC_PATH/mpassthruwindow.h \

SOURCES += \
    ut_passthroughserver.cpp \


CONFIG += debug meegotouch

CONFIG += debug meegotouch
QT += testlib

isEqual(code_coverage_option, off) {
    SOURCES += \
        $$SRC_PATH/mpassthruwindow.cpp
}else {
    QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
    # requires to run make (for $$SRC_PATH) before make check:
    LIBS += $$SRC_PATH/mpassthruwindow.o
}


include(../common_check.pri)
