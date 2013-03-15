# Use when a .pro file requires libmaliit-connection
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

include($$PWD/../common/libmaliit-common.pri)
include($$PWD/../weston-protocols/libmaliit-weston-protocols.pri)

LIBS += $$TOP_DIR/lib/$$maliitStaticLib($${MALIIT_CONNECTION_LIB})
POST_TARGETDEPS += $$TOP_DIR/lib/$$maliitStaticLib($${MALIIT_CONNECTION_LIB})
INCLUDEPATH += $$TOP_DIR/connection $$OUT_PWD/$$TOP_DIR/connection

QT += dbus

wayland {
    CONFIG += link_pkgconfig
    PKGCONFIG += wayland-client
}
