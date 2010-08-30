include(../common_top.pri)
OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../src \
               ../../passthroughserver \
               ../stubs \

SRC_DIR = ../../src


# Input
HEADERS += \
    ut_mimapplication.h \
    ../stubs/mgconfitem_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/minputcontextconnection_stub.h \

SOURCES += \
    ut_mimapplication.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/minputcontextconnection_stub.cpp \

isEqual(code_coverage_option, off){
HEADERS += \
    $$SRC_DIR/mpassthruwindow.h \
SOURCES += \
    $$SRC_DIR/mpassthruwindow.cpp \
}else{
    LIBS += \
        ../../passthroughserver/moc_mpassthruwindow.o \
        ../../passthroughserver/mpassthruwindow.o \
}


CONFIG += debug plugin meegotouch qdbus
QT += testlib

LIBS += \
    ../../src/libmeegoimframework.so.0 \

include(../common_check.pri)
