OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../input-context \
               ../stubs
LIBS += -L../../input-context -lduiinputcontext


HEADERS += \
    ut_duiinputcontextadaptor.h \
    ../stubs/duiinputcontext_stub.h \
    ../stubs/stubbase.h \
    ../stubs/methodcall.h \
    ../stubs/parameter.h

SOURCES += \
    ut_duiinputcontextadaptor.cpp \
    ../stubs/stubbase.cpp


CONFIG +=  debug dui qdbus
QT += testlib

include(../common_check.pri)
