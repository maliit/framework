include(../common_top.pri)

QT += $$QT_WIDGETS

include(../../src/libmaliit-plugins.pri)

# Input
HEADERS += \
    ut_mimapplication.h \

SOURCES += \
    ut_mimapplication.cpp \

CONFIG += plugin qdbus

LIBS += \
        -lXfixes

include(../common_check.pri)
