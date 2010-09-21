include(../common_top.pri)

INCLUDEPATH += ../stubs \
               ../dummyimplugin \
               ../dummyimplugin3 \

# Input
HEADERS += \
    ut_mimpluginmanager.h \
    ../stubs/mgconfitem_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/minputcontextconnection_stub.h \
    $$SRC_DIR/mimpluginmanager_p.h \
    $$SRC_DIR/minputmethodplugin.h \
    $$SRC_DIR/minputcontextconnection.h \

SOURCES += \
    ut_mimpluginmanager.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/minputcontextconnection_stub.cpp \
    $$SRC_DIR/minputcontextconnection.cpp \


CONFIG += debug plugin meegotouch qdbus

LIBS += \
    ../../src/libmeegoimframework.so.0 \
    -L ../plugins/ \
    -ldummyimplugin \
    -ldummyimplugin3 \

target.files += \
    $$TARGET \
    toolbar1.xml \
    toolbar2.xml \

include(../common_check.pri)
