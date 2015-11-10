# Use when a .pro file requires libmaliit-glib
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

LIBS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_GLIB_LIB})
POST_TARGETDEPS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_GLIB_LIB})
INCLUDEPATH += $$TOP_DIR

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0 gobject-2.0
