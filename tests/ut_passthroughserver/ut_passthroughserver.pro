include(../common_top.pri)

QT += $$QT_WIDGETS

LIBS += $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so -lXfixes

# Input
HEADERS += \
    ut_passthroughserver.h \

SOURCES += \
    ut_passthroughserver.cpp \

include(../common_check.pri)
