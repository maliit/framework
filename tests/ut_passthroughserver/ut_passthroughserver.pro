include(../common_top.pri)

QT += $$QT_WIDGETS

include(../../src/libmaliit-plugins.pri)

LIBS += -lXfixes

# Input
HEADERS += \
    ut_passthroughserver.h \

SOURCES += \
    ut_passthroughserver.cpp \

include(../common_check.pri)
