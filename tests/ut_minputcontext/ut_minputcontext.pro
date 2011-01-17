include(../common_top.pri)

ICDIR = ../../input-context
INCLUDEPATH += $$ICDIR ../stubs
LIBS += -L$$ICDIR -lminputcontext ../../src/libmeegoimframework.so
DEFINES += M_LIBRARY=

# Input
HEADERS += \
    ut_minputcontext.h \
    ../stubs/stubbase.h \
    $$ICDIR/minputcontext.h

SOURCES += \
    ut_minputcontext.cpp \
    $$ICDIR/minputcontext.cpp


CONFIG += plugin meegotouch qdbus link_pkgconfig

PKGCONFIG += dbus-glib-1

include(../common_check.pri)
