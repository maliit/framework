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
    CONFIG += meegoimframework meegoimquick
    target.path += $$system(pkg-config --variable pluginsdir MeegoImFramework)
    INSTALLS += target
}

contains(BUILD_TYPE, unittest) {
    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    QUICK_DIR = ../../../input-method-quick
    LIBS += $$QUICK_DIR/libmeegoimquick.so.0
    INCLUDEPATH += $$QUICK_DIR

    FRAMEWORK_DIR = ../../../src
    LIBS += -L$$FRAMEWORK_DIR -lmeegoimframework
    INCLUDEPATH += $$FRAMEWORK_DIR
}

QMAKE_CLEAN += libhelloworldplugin.so
