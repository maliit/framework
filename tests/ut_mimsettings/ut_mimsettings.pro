include(../common_top.pri)

# Input
HEADERS += \
    ut_mimsettings.h \

SOURCES += \
    ut_mimsettings.cpp \

include($$TOP_DIR/src/libmaliit-plugins.pri)
include($$TOP_DIR/connection/libmaliit-connection.pri)

include(../common_check.pri)
