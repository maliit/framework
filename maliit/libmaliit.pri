# Use when a .pro file requires libmaliit
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

include($$TOP_DIR/connection/libmaliit-connection.pri)

LIBS += $$TOP_DIR/maliit/$$maliitDynamicLib($${MALIIT_LIB})
POST_TARGETDEPS += $$TOP_DIR/maliit/$$maliitDynamicLib($${MALIIT_LIB})
INCLUDEPATH += $$TOP_DIR
