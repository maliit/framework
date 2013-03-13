include(../../config.pri)

TOP_DIR = ../..

TEMPLATE = lib
TARGET = ../plugins/dummyimplugin2
DEPENDPATH += .

include($$TOP_DIR/common/libmaliit-common.pri)
include($$TOP_DIR/src/libmaliit-plugins.pri)

CONFIG += plugin

HEADERS += dummyimplugin2.h
SOURCES += dummyimplugin2.cpp
OTHER_FILES += dummyimplugin2.json

target.path += $$MALIIT_TEST_LIBDIR/plugins

INSTALLS += target

QMAKE_CLEAN += ../plugins/libdummyimplugin2.so

QMAKE_EXTRA_TARGETS += check
check.target = check
check.command = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.command = $$system(true)

QMAKE_EXTRA_TARGETS += memcheck
memcheck.target = memcheck
memcheck.command = $$system(true)
