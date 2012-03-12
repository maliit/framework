include(../common_top.pri)

QT += $$QT_WIDGETS

SOURCES += ut_maliit_inputmethod.cpp \

LIBS += $$TOP_DIR/maliit/libmaliit-1.0.so

include(../common_check.pri)
