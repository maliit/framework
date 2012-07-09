# Use when a .pro file requires libmaliit-common
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

INCLUDEPATH += $$TOP_DIR/common
LIBS += $$TOP_DIR/lib/$$maliitStaticLib(maliit-common)
POST_TARGETDEPS += $$TOP_DIR/lib/$$maliitStaticLib(maliit-common)
