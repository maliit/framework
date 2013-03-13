include(../../config.pri)

TOP_DIR = ../..

TEMPLATE = lib
TARGET = ../plugins/dummyimplugin3
DEPENDPATH += .

include($$TOP_DIR/common/libmaliit-common.pri)
include($$TOP_DIR/src/libmaliit-plugins.pri)

CONFIG += plugin

HEADERS += \
    dummyimplugin3.h \
    dummyinputmethod3.h \

SOURCES += \
    dummyimplugin3.cpp \
    dummyinputmethod3.cpp \

OTHER_FILES += \
    dummyimplugin3.json \

target.path += $$MALIIT_TEST_LIBDIR/plugins

INSTALLS += target

QMAKE_CLEAN += ../plugins/libdummyimplugin3.so

QMAKE_EXTRA_TARGETS += check
check.target = check
check.command = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.command = $$system(true)

QMAKE_EXTRA_TARGETS += memcheck
memcheck.target = memcheck
memcheck.command = $$system(true)
