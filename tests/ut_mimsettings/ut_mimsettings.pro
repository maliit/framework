include(../common_top.pri)

# Input
HEADERS += \
    ut_mimsettings.h \

SOURCES += \
    ut_mimsettings.cpp \

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \

include(../common_check.pri)
