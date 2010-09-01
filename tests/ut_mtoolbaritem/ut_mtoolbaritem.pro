include(../common_top.pri)

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mtoolbaritem.h \
    ../stubs/mgconfitem_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/minputcontextconnection_stub.h \

SOURCES += \
    ut_mtoolbaritem.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/minputcontextconnection_stub.cpp \

isEqual(code_coverage_option, off){
HEADERS += \
    $$SRC_DIR/mtoolbaritem.h \
    $$SRC_DIR/minputmethodnamespace.h \

SOURCES += \
    $$SRC_DIR/mtoolbaritem.cpp \
}

CONFIG += debug plugin meegotouch qdbus

LIBS += \
    ../../src/libmeegoimframework.so.0 \

include(../common_check.pri)
