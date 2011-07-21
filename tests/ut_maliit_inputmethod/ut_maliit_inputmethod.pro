include(../common_top.pri)

SOURCES += ut_maliit_inputmethod.cpp \

LIBS += \
    $$TOP_DIR/maliit/libmaliit-1.0.so

include(../common_check.pri)
