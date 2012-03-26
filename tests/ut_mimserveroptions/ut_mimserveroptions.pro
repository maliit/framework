include(../common_top.pri)

# Input
HEADERS += \
    ut_mimserveroptions.h \

SOURCES += \
    ut_mimserveroptions.cpp \

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \

include(../common_check.pri)
