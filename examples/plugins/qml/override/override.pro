TEMPLATE = lib
TARGET = qmloverrideplugin

OBJECTS_DIR = .obj
MOC_DIR = .moc

CONFIG += debug plugin

contains(QT_MAJOR_VERSION, 4) {
    QT = core gui declarative
} else {
    QT = core gui widgets declarative
}

SOURCES += overrideplugin.cpp
HEADERS += overrideplugin.h
RESOURCES = override.qrc
target.depends = $$IN_PWD/override.qml

OTHER_FILES += \
    override.qml \

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-plugins-quick-0.80
    target.path += $$system(pkg-config --variable pluginsdir maliit-plugins-0.80)
    INCLUDEPATH += $$system(pkg-config --cflags maliit-plugins-quick-0.80 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
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
    TOP_DIR = ../../../..

    include($$TOP_DIR/config.pri)

    INCLUDEPATH += $$TOP_DIR

    QUICK_DIR = $$TOP_DIR/maliit-plugins-quick/input-method
    LIBS += $$QUICK_DIR/lib$${MALIIT_PLUGINS_QUICK_LIB}.so
    INCLUDEPATH += $$QUICK_DIR

    SRC_DIR = $$TOP_DIR/src
    LIBS += $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so
    INCLUDEPATH += $$SRC_DIR $$TOP_DIR/common

    target.path += $$MALIIT_TEST_PLUGINS_DIR/examples/qml/override
    INSTALLS += target
}

QMAKE_CLEAN += libqmloverrideplugin.so
