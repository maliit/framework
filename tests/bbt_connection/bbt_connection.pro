include(../common_top.pri)

IC_DIR = $$SRC_DIR/../input-context

# Input
HEADERS += \
    bbt_connection.h \

SOURCES += \
    bbt_connection.cpp \

include(../../connection/libmaliit-connection.pri)

include(../common_check.pri)
