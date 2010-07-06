OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../input-context
LIBS += -L../../input-context -lminputcontext


HEADERS += \
    ut_minputcontextplugin.h

SOURCES += \
    ut_minputcontextplugin.cpp


CONFIG += debug meegotouch
QT += testlib

include(../common_check.pri)
