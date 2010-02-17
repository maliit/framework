OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../input-context
LIBS += -L../../input-context -lduiinputcontext


HEADERS += \
    ut_duiinputcontextplugin.h

SOURCES += \
    ut_duiinputcontextplugin.cpp


CONFIG +=  debug dui
QT += testlib

include(../common_check.pri)
