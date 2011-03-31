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
    FRAMEWORK_DIR = ../../../src
    LIBS += -L$$FRAMEWORK_DIR -lmeegoimframework
    INCLUDEPATH += $$FRAMEWORK_DIR
}

QMAKE_CLEAN += libexampleplugin.so
