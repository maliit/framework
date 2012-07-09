# Use when a .pro file requires libmaliit
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

include($$IN_PWD/../connection/libmaliit-connection.pri)

LIBS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_LIB})
POST_TARGETDEPS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_LIB})
INCLUDEPATH += $$TOP_DIR
