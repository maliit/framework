include(../common_top.pri)

INCLUDEPATH += ../stubs \
               ../dummyimplugin \

SRC_DIR = ../../src


# Input
HEADERS += \
    ft_mimpluginmanager.h \
    ../stubs/mgconfitem_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/fakeproperty.h \
    ../stubs/mkeyboardstatetracker_stub.h \
    ../stubs/minputcontextconnection_stub.h \

SOURCES += \
    ft_mimpluginmanager.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/fakeproperty.cpp \
    ../stubs/minputcontextconnection_stub.cpp \


CONFIG += debug plugin meegotouch qdbus

LIBS += \
    ../../src/libmeegoimframework.so \
    -L ../plugins/ \
    -ldummyimplugin \

include(../common_check.pri)
