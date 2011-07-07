include(../common_top.pri)

INCLUDEPATH += ../../input-context
LIBS += -L../../input-context -lminputcontext

LIBS += ../../maliit/lib$${MALIIT_LIB}.so

nomeegotouch  {
    DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"Maliit\\\"
} else {
    DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"MInputContext\\\"
}

HEADERS += \
    ut_minputcontextplugin.h

SOURCES += \
    ut_minputcontextplugin.cpp


CONFIG += meegotouch

include(../common_check.pri)
