include(../common_top.pri)

QT += $$QT_WIDGETS

IC_DIR = $$SRC_DIR/../input-context

# Input
HEADERS += \
    bbt_connection.h \

SOURCES += \
    bbt_connection.cpp \

include(../../connection/libmaliit-connection.pri)

needs_x = yes

include(../common_check.pri)
