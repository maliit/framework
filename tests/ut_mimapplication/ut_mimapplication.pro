include(../common_top.pri)

INCLUDEPATH += ../../passthroughserver \
               ../stubs \

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

LIBS += \
    ../../src/libmeegoimframework.so.0 \

include(../common_check.pri)
