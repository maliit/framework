include(../common_top.pri)

INCLUDEPATH += ../../input-context
LIBS += -L../../input-context -lminputcontext


HEADERS += \
    ut_minputcontextplugin.h

SOURCES += \
    ut_minputcontextplugin.cpp


CONFIG += meegotouch

include(../common_check.pri)
