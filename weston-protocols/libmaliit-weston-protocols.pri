# Use when a .pro file requires libmaliit-weston-protocols
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

wayland {
    LIBS += $$TOP_DIR/lib/$$maliitStaticLib(maliit-weston-protocols)
    POST_TARGETDEPS += $$TOP_DIR/lib/$$maliitStaticLib(maliit-weston-protocols)
    INCLUDEPATH += $$TOP_DIR/weston-protocols $$OUT_PWD/$$TOP_DIR/weston-protocols

    CONFIG += link_pkgconfig
    PKGCONFIG += wayland-client
}
