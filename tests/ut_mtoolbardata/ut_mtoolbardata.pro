include(../common_top.pri)

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mtoolbardata.h \
    ../stubs/mgconfitem_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/minputcontextconnection_stub.h \

SOURCES += \
    ut_mtoolbardata.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/minputcontextconnection_stub.cpp \

isEqual(code_coverage_option, off){
HEADERS += \
    $$SRC_DIR/mtoolbaritem.h \
    $$SRC_DIR/mtoolbardata.h \
    $$SRC_DIR/minputmethodnamespace.h \

SOURCES += \
    $$SRC_DIR/mtoolbaritem.cpp \
    $$SRC_DIR/mtoolbardata.cpp \
}


CONFIG += debug plugin meegotouch qdbus

LIBS += \
    ../../src/libmeegoimframework.so.0 \

target.files += \
    $$TARGET \
    toolbar1.xml \
    toolbar2.xml \
    toolbar3.xml \

include(../common_check.pri)
