include(../common_top.pri)

INCLUDEPATH += \
    $$SRC_DIR \

# Input
HEADERS += \
    ut_selfcompositing.h \

SOURCES += \
    ut_selfcompositing.cpp \

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so -lXcomposite -lXdamage -lXfixes

include(../common_check.pri)
