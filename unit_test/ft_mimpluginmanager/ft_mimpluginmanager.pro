OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../src \
               ../stubs \

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
QT += testlib

LIBS += \
    ../../src/libmeegoimframework.so \

include(../common_check.pri)
