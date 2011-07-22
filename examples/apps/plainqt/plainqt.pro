TEMPLATE = app
TARGET = maliit-exampleapp-plainqt
target.path = /usr/bin
DEPENDPATH += .

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-1.0
    INCLUDEPATH += $$system(pkg-config --cflags maliit-1.0 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)

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

SOURCES += \
           plainqt.cpp \
           mainwindow.cpp \

HEADERS += \
           mainwindow.h \

QT += core gui

INSTALLS += target
QMAKE_CLEAN += maliit-exampleapp-plainqt
