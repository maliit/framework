TEMPLATE = lib
TARGET = cxxhelloworldplugin

OBJECTS_DIR = .obj
MOC_DIR = .moc

CONFIG += debug plugin

contains(QT_MAJOR_VERSION, 4) {
    QT += core gui
} else {
    QT += core gui widgets
}

HEADERS += \
    helloworldplugin.h \
    helloworldinputmethod.h \

SOURCES += \
    helloworldplugin.cpp \
    helloworldinputmethod.cpp \

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-plugins-1.0
    target.path += $$system(pkg-config --variable pluginsdir maliit-plugins-1.0)
    INCLUDEPATH += $$system(pkg-config --cflags maliit-plugins-1.0 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
    INSTALLS += target
}

contains(BUILD_TYPE, unittest) {
    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    TOP_DIR = ../../../..

    include($$TOP_DIR/config.pri)

    include($$TOP_DIR/common/libmaliit-common.pri)
    include($$TOP_DIR/src/libmaliit-plugins.pri)

    target.path += $$MALIIT_TEST_PLUGINS_DIR/examples/cxx/helloworld
    INSTALLS += target
}

QMAKE_CLEAN += libcxxhelloworldplugin.so
