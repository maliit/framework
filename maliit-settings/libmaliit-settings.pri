# Use when a .pro file requires libmaliit-settings
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

include ($$PWD/../connection/libmaliit-connection.pri)
include ($$PWD/../maliit/libmaliit.pri)

LIBS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_SETTINGS_LIB})
POST_TARGETDEPS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_SETTINGS_LIB})
INCLUDEPATH += $$TOP_DIR/maliit-settings
