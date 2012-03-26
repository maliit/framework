include(../common_top.pri)

QT += $$QT_WIDGETS

INCLUDEPATH += ../../src \

# Input
HEADERS += \
    ut_mimapplication.h \

SOURCES += \
    ut_mimapplication.cpp \

CONFIG += plugin qdbus

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so -lXfixes

include(../common_check.pri)
