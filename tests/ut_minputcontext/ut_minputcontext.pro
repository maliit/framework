include(../common_top.pri)

QT += $$QT_WIDGETS

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

# Input
HEADERS += \
    ut_minputcontext.h \

SOURCES += \
    ut_minputcontext.cpp \

include($$TOP_DIR/maliit/libmaliit.pri)
include($$TOP_DIR/common/libmaliit-common.pri)
include($$TOP_DIR/input-context/libmaliit-inputcontext-qt4.pri)
include($$TOP_DIR/connection/libmaliit-connection.pri)

needs_x = yes

include(../common_check.pri)
