OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../src ../../input-context ../stubs
LIBS += -L../../input-context -lduiinputcontext ../../src/libduiimframework.so \

# Input
HEADERS += \
    ut_duiinputcontext.h \
    ../stubs/stubbase.h \

SOURCES += \
    ut_duiinputcontext.cpp \


CONFIG +=  debug plugin dui qdbus
QT += testlib

include(../common_check.pri)
