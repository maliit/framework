include(../common_top.pri)

IC_DIR = $$SRC_DIR/../input-context

# Input
HEADERS += \
    bbt_connection.h \

SOURCES += \
    bbt_connection.cpp \

LIBS += ../../connection/libmaliit-connection.a
POST_TARGETDEPS += ../../connection/libmaliit-connection.a

include(../common_check.pri)
