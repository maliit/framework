TEMPLATE = app
TARGET = maliit-exampleapp-gtk3

OBJECTS_DIR = .obj

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-glib-1.0
    INCLUDEPATH += $$system(pkg-config --cflags maliit-glib-1.0 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    target.path = $$M_IM_INSTALL_BIN
    INSTALLS += target

    TOP_DIR = ../../..
    include($$TOP_DIR/maliit-glib/libmaliit-glib.pri)
}

CONFIG -= qt
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-3.0

SOURCES = ../gtk2/exampleapp-gtk.c

QMAKE_CLEAN += maliit-exampleapp-gtk3
