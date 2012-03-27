include(../common_top.pri)

QT += $$QT_WIDGETS

ICDIR = ../../input-context/maliit-input-context
INCLUDEPATH += $$ICDIR ../stubs
LIBS += -L$$ICDIR -l$$MALIIT_INPUTCONTEXT_TARGETNAME $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so
DEFINES += M_LIBRARY=

INCLUDEPATH += ../../maliit
LIBS += ../../maliit/lib$${MALIIT_LIB}.so

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

# Input
HEADERS += \
    ut_minputcontext.h \
    $$ICDIR/minputcontext.h

SOURCES += \
    ut_minputcontext.cpp \
    $$ICDIR/minputcontext.cpp

include($$TOP_DIR/connection/libmaliit-connection.pri)

CONFIG += plugin qdbus link_pkgconfig

include(../common_check.pri)
