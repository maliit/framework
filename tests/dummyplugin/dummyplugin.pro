include(../../config.pri)

TOP_DIR = ../..

TEMPLATE = lib
TARGET = ../plugins/dummyplugin
DEPENDPATH += .

include($$TOP_DIR/common/libmaliit-common.pri)
include($$TOP_DIR/src/libmaliit-plugins.pri)

CONFIG += plugin

HEADERS += dummyplugin.h
SOURCES += dummyplugin.cpp
OTHER_FILES += dummyplugin.json

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
