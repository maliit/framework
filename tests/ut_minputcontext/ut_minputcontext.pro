include(../common_top.pri)

ICDIR = ../../input-context
INCLUDEPATH += $$ICDIR ../stubs
LIBS += -L$$ICDIR -l$$MALIIT_INPUTCONTEXT_TARGETNAME $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so
DEFINES += M_LIBRARY=

external-libmaliit {
    PKGCONFIG += maliit-1.0
} else {
    INCLUDEPATH += ../../maliit
    LIBS += ../../maliit/lib$${MALIIT_LIB}.so
}

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

# Input
HEADERS += \
    ut_minputcontext.h \
    $$ICDIR/minputcontext.h

SOURCES += \
    ut_minputcontext.cpp \
    $$ICDIR/minputcontext.cpp


CONFIG += plugin qdbus link_pkgconfig

PKGCONFIG += dbus-glib-1

include(../common_check.pri)
