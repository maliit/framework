include(../common_top.pri)
OBJECTS_DIR = .obj
MOC_DIR = .moc

SRC_PATH = ../../passthroughserver
INCLUDEPATH += $$SRC_PATH

LIBS += ../../src/libmeegoimframework.so -lXfixes


# Input
HEADERS += \
    ut_passthroughserver.h \

SOURCES += \
    ut_passthroughserver.cpp \


CONFIG += debug meegotouch

isEqual(code_coverage_option, off){
HEADERS += \
    $$SRC_PATH/mpassthruwindow.h \
SOURCES += \
    $$SRC_PATH/mpassthruwindow.cpp \
}else{
    QMAKE_CXXFLAGS += -fno-elide-constructors
    LIBS += \
    ../../passthroughserver/moc_mpassthruwindow.o \
    ../../passthroughserver/mpassthruwindow.o \
}

include(../common_check.pri)
