# Use when a .pro file requires libmaliit-settings
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

LIBS += $$TOP_DIR/maliit-settings/$$maliitDynamicLib($${MALIIT_SETTINGS_LIB}) $$TOP_DIR/connection/$$maliitDynamicLib($${MALIIT_CONNECTION_LIB})
POST_TARGETDEPS += $$TOP_DIR/maliit-settings/$$maliitDynamicLib($${MALIIT_SETTINGS_LIB})
INCLUDEPATH += $$TOP_DIR/maliit-settings
