OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../../src ../../input-context ../stubs
LIBS += -L../../input-context -lminputcontext ../../src/libmeegoimframework.so \

# Input
HEADERS += \
    ut_minputcontext.h \
    ../stubs/stubbase.h \

SOURCES += \
    ut_minputcontext.cpp \


CONFIG += debug plugin meegotouch qdbus link_pkgconfig
QT += testlib

PKGCONFIG += dbus-glib-1

include(../common_check.pri)
