OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../input-context \
               ../stubs
LIBS += -L../../input-context -lminputcontext


HEADERS += \
    ut_minputcontextadaptor.h \
    ../stubs/minputcontext_stub.h \
    ../stubs/stubbase.h \
    ../stubs/methodcall.h \
    ../stubs/parameter.h

SOURCES += \
    ut_minputcontextadaptor.cpp \
    ../stubs/stubbase.cpp


CONFIG += debug meegotouch qdbus
QT += testlib

include(../common_check.pri)
