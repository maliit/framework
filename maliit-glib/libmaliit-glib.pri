# Use when a .pro file requires libmaliit
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

LIBS += $$TOP_DIR/maliit-glib/$$maliitDynamicLib($${MALIIT_GLIB_LIB})
POST_TARGETDEPS += $$TOP_DIR/maliit-glib/$$maliitDynamicLib($${MALIIT_GLIB_LIB})
INCLUDEPATH += $$TOP_DIR