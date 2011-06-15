include(../../../config.pri)

TEMPLATE = lib
TARGET = exampleplugin

OBJECTS_DIR = .obj
MOC_DIR = .moc

CONFIG += debug plugin

HEADERS += \
    exampleplugin.h \
    exampleinputmethod.h \

SOURCES += \
    exampleplugin.cpp \
    exampleinputmethod.cpp \

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += meegoimframework
    target.path += $$system(pkg-config --variable pluginsdir MeegoImFramework)
    INSTALLS += target
}

contains(BUILD_TYPE, unittest) {
    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    SRC_DIR = ../../../src
    LIBS += $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so
    INCLUDEPATH += $$SRC_DIR
}

QMAKE_CLEAN += libexampleplugin.so
