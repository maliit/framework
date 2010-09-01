include(../common_top.pri)

INCLUDEPATH += ../../input-context ../stubs
LIBS += -L../../input-context -lminputcontext ../../src/libmeegoimframework.so \

# Input
HEADERS += \
    ut_minputcontext.h \
    ../stubs/stubbase.h \

SOURCES += \
    ut_minputcontext.cpp \


CONFIG += debug plugin meegotouch qdbus link_pkgconfig

PKGCONFIG += dbus-glib-1

include(../common_check.pri)
