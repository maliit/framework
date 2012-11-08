TEMPLATE = lib
TARGET = qmlcyclekeysplugin

OBJECTS_DIR = .obj
MOC_DIR = .moc

CONFIG += debug plugin

contains(QT_MAJOR_VERSION, 4) {
    QT = core gui declarative
} else {
    QT = core gui widgets declarative
}

SOURCES += cyclekeysplugin.cpp
HEADERS += cyclekeysplugin.h
RESOURCES = cyclekeys.qrc

OTHER_FILES += \
    main.qml \
    CycleKey.qml \

target.depends = $${PWD}/main.qml $${PWD}/CycleKey.qml

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-plugins-quick-0.80
    target.path += $$system(pkg-config --variable pluginsdir maliit-plugins-1.0)
    INCLUDEPATH += $$system(pkg-config --cflags maliit-plugins-quick-0.80 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
    INSTALLS += target
}

contains(BUILD_TYPE, unittest) {
    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    TOP_DIR = ../../../..

    include($$TOP_DIR/config.pri)

    include($$TOP_DIR/common/libmaliit-common.pri)
    include($$TOP_DIR/src/libmaliit-plugins.pri)
    include($$TOP_DIR/maliit-plugins-quick/libmaliit-plugins-quick.pri)
    
    target.path += $$MALIIT_TEST_PLUGINS_DIR/examples/qml/cyclekeys
    INSTALLS += target
}

QMAKE_CLEAN += libqmlcyclekeysplugin.so
