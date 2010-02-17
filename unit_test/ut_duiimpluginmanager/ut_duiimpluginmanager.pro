OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../src \
               ../stubs \
               ../dummyimplugin \
               ../dummyimplugin3 \

SRC_DIR = ../../src


# Input
HEADERS += \
    ut_duiimpluginmanager.h \
    ../stubs/duigconfitem_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/duiinputcontextconnection_stub.h \
    $$SRC_DIR/duiimpluginmanager_p.h \
    $$SRC_DIR/duiinputmethodplugin.h \
    $$SRC_DIR/duiinputcontextconnection.h \

SOURCES += \
    ut_duiimpluginmanager.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/duiinputcontextconnection_stub.cpp \
    $$SRC_DIR/duiimpluginmanager_p.cpp \
    $$SRC_DIR/duiinputcontextconnection.cpp \


CONFIG +=  debug plugin dui qdbus
QT += testlib

LIBS += \
    ../../src/libduiimframework.so.0 \
    -L ../plugins/ \
    -ldummyimplugin \
    -ldummyimplugin3 \

include(../common_check.pri)
