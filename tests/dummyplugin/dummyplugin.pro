include(../../config.pri)

TEMPLATE = lib
TARGET = ../plugins/$$qtLibraryTarget(dummyplugin)
DEPENDPATH += .
SRC_DIR = ../../src
INCLUDEPATH += . ../.. $$SRC_DIR ../../common
LIBS += $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so

OBJECTS_DIR = .obj
MOC_DIR = .moc

CONFIG += debug plugin 

HEADERS += dummyplugin.h
SOURCES += dummyplugin.cpp

target.path += $$MALIIT_TEST_LIBDIR/plugins

INSTALLS += target

QMAKE_CLEAN += ../plugins/libdummyplugin.so

QMAKE_EXTRA_TARGETS += check
check.target = check
check.command = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.command = $$system(true)

QMAKE_EXTRA_TARGETS += memcheck
memcheck.target = memcheck
memcheck.command = $$system(true)
