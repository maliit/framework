include(../common_top.pri)

INCLUDEPATH += ../../input-context
LIBS += -L../../input-context -l$$MALIIT_INPUTCONTEXT_TARGETNAME

external-libmaliit {
    PKGCONFIG += maliit-1.0
} else {
    LIBS += ../../maliit/lib$${MALIIT_LIB}.so
}

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

HEADERS += \
    ut_minputcontextplugin.h

SOURCES += \
    ut_minputcontextplugin.cpp


CONFIG += link_pkgconfig

include(../common_check.pri)
