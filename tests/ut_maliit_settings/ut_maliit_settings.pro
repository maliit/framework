include(../common_top.pri)

SOURCES += ut_maliit_settings.cpp

LIBS += $$TOP_DIR/maliit/libmaliit-1.0.so $$TOP_DIR/maliit-settings/libmaliit-settings-1.0.so $$TOP_DIR/connection/libmaliit-connection-0.80.so

include(../common_check.pri)
