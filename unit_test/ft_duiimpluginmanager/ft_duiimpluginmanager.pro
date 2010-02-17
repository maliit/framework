OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../src \
               ../stubs \

SRC_DIR = ../../src


# Input
HEADERS += \
    ft_duiimpluginmanager.h \
    ../stubs/duigconfitem_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/duiinputcontextconnection_stub.h \

SOURCES += \
    ft_duiimpluginmanager.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/duiinputcontextconnection_stub.cpp \


CONFIG +=  debug plugin dui qdbus
QT += testlib

LIBS += \
    ../../src/libduiimframework.so \

include(../common_check.pri)
