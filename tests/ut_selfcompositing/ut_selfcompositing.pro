include(../common_top.pri)

QT += $$QT_WIDGETS

include(../../src/libmaliit-plugins.pri)

# Input
HEADERS += \
    ut_selfcompositing.h \
    ../utils/gui-utils.h \

SOURCES += \
    ut_selfcompositing.cpp \
    ../utils/gui-utils.cpp \

LIBS += \
    -lXcomposite -lXdamage -lXfixes

include(../common_check.pri)
