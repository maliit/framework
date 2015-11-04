# Use when a .pro file requires libmaliit-connection-glib
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

LIBS += $$TOP_DIR/lib/$$maliitStaticLib($${MALIIT_CONNECTION_GLIB_LIB})
POST_TARGETDEPS += $$TOP_DIR/lib/$$maliitStaticLib($${MALIIT_CONNECTION_GLIB_LIB})
INCLUDEPATH += $$TOP_DIR/connection-glib
PKGCONFIG += glib-2.0

system(pkg-config gio-2.0 --atleast-version 2.26) {
    PKGCONFIG += gio-2.0
}
