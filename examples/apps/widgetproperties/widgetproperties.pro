TEMPLATE = app
TARGET = maliit-exampleapp-widgetproperties
DEPENDPATH += .

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-1.0
    INCLUDEPATH += $$system(pkg-config --cflags maliit-1.0 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    target.path = $$M_IM_INSTALL_BIN
    INSTALLS += target

    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    external-libmaliit {
        CONFIG += link_pkgconfig
        PKGCONFIG += maliit-1.0
    } else {
        TOP_DIR = ../../..
        INCLUDEPATH += $$TOP_DIR $$TOP_DIR/common
        LIBS += $$TOP_DIR/maliit/lib$${MALIIT_LIB}.so
    }
}

SOURCES += \
    mainwindow.cpp \
    widgetproperties.cpp \

HEADERS += \
    mainwindow.h \

QT += core gui

QMAKE_CLEAN += target
