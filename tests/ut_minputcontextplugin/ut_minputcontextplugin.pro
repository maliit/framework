include(../common_top.pri)

INCLUDEPATH += ../../input-context
LIBS += -L../../input-context -lminputcontext

external-libmaliit {
    PKGCONFIG += maliit-1.0
} else {
    LIBS += ../../maliit/lib$${MALIIT_LIB}.so
}

!enable-legacy  {
    DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"Maliit\\\"
} else {
    DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"MInputContext\\\"
}

HEADERS += \
    ut_minputcontextplugin.h

SOURCES += \
    ut_minputcontextplugin.cpp


CONFIG += link_pkgconfig

include(../common_check.pri)
