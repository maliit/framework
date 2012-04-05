# Use when a .pro file requires libmaliit-plugins-quick-factory
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory

LIBS += $$TOP_DIR/maliit-plugins-quick/plugin-factory/$$maliitDynamicLib($${MALIIT_PLUGINS_QUICK_FACTORY})
POST_TARGETDEPS += $$TOP_DIR/maliit-plugins-quick/plugin-factory/$$maliitDynamicLib($${MALIIT_PLUGINS_QUICK_FACTORY})
INCLUDEPATH += $$TOP_DIR/maliit-plugins-quick/plugin-factory
