include(../common_top.pri)

QT += $$QT_WIDGETS

SOURCES += ut_maliit_inputmethod.cpp \

include ($$TOP_DIR/maliit/libmaliit.pri)

needs_x = yes

include(../common_check.pri)
