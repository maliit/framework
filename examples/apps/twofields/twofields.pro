include(../../../config.pri)

TEMPLATE = app
TARGET = maliit-exampleapp-twofields
target.path = $$M_IM_INSTALL_BIN
DEPENDPATH += .

BUILD_TYPE = skeleton

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-1.0
    INCLUDEPATH += $$system(pkg-config --cflags maliit-1.0 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
}

contains(BUILD_TYPE, unittest) {

    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    external-libmaliit {
        CONFIG += link_pkgconfig
        PKGCONFIG += maliit-1.0
    } else {
        TOP_SRC = ../../..
        INCLUDEPATH += $$TOP_SRC
        LIBS += $$TOP_SRC/maliit/lib$${MALIIT_LIB}.so
    }
}

SOURCES +=          \
    twofields.cpp   \
    mainwindow.cpp

HEADERS +=          \
    mainwindow.h

QT += core gui

INSTALLS += target
QMAKE_CLEAN += maliit-exampleapp-twofields
