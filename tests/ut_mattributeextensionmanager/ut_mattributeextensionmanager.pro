include(../common_top.pri)

# Input
HEADERS += \
    ut_mattributeextensionmanager.h \

SOURCES += \
    ut_mattributeextensionmanager.cpp \

CONFIG += plugin qdbus

include($$TOP_DIR/src/libmaliit-plugins.pri)
include($$TOP_DIR/connection/libmaliit-connection.pri)

target.files += \
    $$TARGET \
    toolbar1.xml \
    toolbar2.xml \

include(../common_check.pri)
