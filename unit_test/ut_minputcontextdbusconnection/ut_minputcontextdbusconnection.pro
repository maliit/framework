OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../src ../../input-context ../stubs
LIBS += -L../../input-context/ -lminputcontext ../../src/libmeegoimframework.so \

# Input
HEADERS += \
    ut_minputcontextdbusconnection.h \
    ../stubs/stubbase.h \
    ../stubs/minputcontextadaptor_stub.h

SOURCES += \
    ut_minputcontextdbusconnection.cpp \
    ../stubs/stubbase.cpp


CONFIG += debug plugin meegotouch qdbus
QT += testlib

include(../common_check.pri)
