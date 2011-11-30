TEMPLATE = app
TARGET = maliit-exampleapp-gtk3

OBJECTS_DIR = .obj

BUILD_TYPE = unittest

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    target.path = $$M_IM_INSTALL_BIN
    INSTALLS += target
}

CONFIG -= qt
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-3.0

SOURCES = ../gtk2/exampleapp-gtk.c

QMAKE_CLEAN += maliit-exampleapp-gtk3
