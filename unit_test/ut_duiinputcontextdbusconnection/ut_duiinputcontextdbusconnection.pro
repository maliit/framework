OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../src ../../input-context ../stubs
LIBS += -L../../input-context/ -lduiinputcontext ../../src/libduiimframework.so \

# Input
HEADERS += \
    ut_duiinputcontextdbusconnection.h \
    ../stubs/stubbase.h \
    ../stubs/duiinputcontextadaptor_stub.h

SOURCES += \
    ut_duiinputcontextdbusconnection.cpp \
    ../stubs/stubbase.cpp


CONFIG +=  debug plugin dui qdbus
QT += testlib

include(../common_check.pri)
