# Use when a .pro file requires libmaliit-plugins-quick
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory

LIBS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_PLUGINS_QUICK_LIB})
POST_TARGETDEPS += $$TOP_DIR/lib/$$maliitDynamicLib($${MALIIT_PLUGINS_QUICK_LIB})
INCLUDEPATH += $$TOP_DIR/maliit-plugins-quick/input-method
