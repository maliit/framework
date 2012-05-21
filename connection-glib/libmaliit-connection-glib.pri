# Use when a .pro file requires libmaliit-connection
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

LIBS += $$TOP_DIR/connection-glib/$$maliitStaticLib($${MALIIT_CONNECTION_GLIB_LIB})
POST_TARGETDEPS += $$TOP_DIR/connection-glib/$$maliitStaticLib($${MALIIT_CONNECTION_GLIB_LIB})
INCLUDEPATH += $$TOP_DIR/connection-glib
PKGCONFIG += glib-2.0 gthread-2.0 dbus-glib-1
