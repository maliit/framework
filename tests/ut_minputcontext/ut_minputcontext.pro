include(../common_top.pri)

ICDIR = ../../input-context
INCLUDEPATH += $$ICDIR ../stubs
LIBS += -L$$ICDIR -lminputcontext $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so -lX11
DEFINES += M_LIBRARY=

nomeegotouch {
    INCLUDEPATH += ../../maliit
    LIBS += ../../maliit/lib$${MALIIT_LIB}.so
}

# Input
HEADERS += \
    ut_minputcontext.h \
    ../stubs/stubbase.h \
    $$ICDIR/minputcontext.h

SOURCES += \
    ut_minputcontext.cpp \
    $$ICDIR/minputcontext.cpp


CONFIG += plugin qdbus link_pkgconfig

PKGCONFIG += dbus-glib-1

include(../common_check.pri)
