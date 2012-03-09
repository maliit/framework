# Use when a .pro file requires libmaliit-connection
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory
LIBS += $$TOP_DIR/connection/lib$${MALIIT_CONNECTION_LIB}.so
POST_TARGETDEPS += $$TOP_DIR/connection/lib$${MALIIT_CONNECTION_LIB}.so
INCLUDEPATH += $$TOP_DIR/connection
