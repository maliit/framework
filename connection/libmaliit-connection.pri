# Use when a .pro file requires libmaliit-connection
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

include($$IN_PWD/../common/libmaliit-common.pri)

LIBS += $$TOP_DIR/connection/$$maliitDynamicLib($${MALIIT_CONNECTION_LIB})
POST_TARGETDEPS += $$TOP_DIR/connection/$$maliitDynamicLib($${MALIIT_CONNECTION_LIB})
INCLUDEPATH += $$TOP_DIR/connection
