TEMPLATE = lib
TARGET = helloworldplugin

OBJECTS_DIR = .obj
MOC_DIR = .moc

CONFIG += debug plugin
QT = core gui declarative

SOURCES += helloworldplugin.cpp
HEADERS += helloworldplugin.h
RESOURCES = helloworld.qrc

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-plugins-quick-0.20
    target.path += $$system(pkg-config --variable pluginsdir maliit-plugins-0.20)
    INCLUDEPATH += $$system(pkg-config --cflags maliit-plugins-quick-0.20 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
    INSTALLS += target
}

contains(BUILD_TYPE, skeleton-legacy) {
    CONFIG += meegoimframework meegoimquick
    target.path += $$system(pkg-config --variable pluginsdir MeegoImFramework)
    INSTALLS += target
}

contains(BUILD_TYPE, unittest) {
    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    include(../../../config.pri)

    QUICK_DIR = ../../../input-method-quick
    LIBS += $$QUICK_DIR/lib$${MALIIT_PLUGINS_QUICK_LIB}.so
    INCLUDEPATH += $$QUICK_DIR

    SRC_DIR = ../../../src
    LIBS += $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so
    INCLUDEPATH += $$SRC_DIR
}

QMAKE_CLEAN += libhelloworldplugin.so
