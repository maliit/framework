# Use when a .pro file requires libmaliit-settings
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

include ($$IN_PWD/../connection/libmaliit-connection.pri)
include ($$IN_PWD/../maliit/libmaliit.pri)

LIBS += $$TOP_DIR/maliit-settings/$$maliitDynamicLib($${MALIIT_SETTINGS_LIB})
POST_TARGETDEPS += $$TOP_DIR/maliit-settings/$$maliitDynamicLib($${MALIIT_SETTINGS_LIB})
INCLUDEPATH += $$TOP_DIR/maliit-settings
