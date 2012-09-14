# Use when a .pro file requires libmaliit-plugins
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

INCLUDEPATH += $$TOP_DIR/src $$OUT_PWD/$$TOP_DIR/src
LIBS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_PLUGINS_LIB})
POST_TARGETDEPS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_PLUGINS_LIB})
