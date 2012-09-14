include(../../config.pri)

TOP_DIR = ../..

TEMPLATE = lib
TARGET = ../plugins/dummyimplugin
DEPENDPATH += .

include($$TOP_DIR/common/libmaliit-common.pri)
include($$TOP_DIR/src/libmaliit-plugins.pri)

CONFIG += plugin

QT += $$QT_WIDGETS

HEADERS += \
    dummyimplugin.h \
    dummyinputmethod.h \

SOURCES += \
    dummyimplugin.cpp \
    dummyinputmethod.cpp \

OTHER_FILES += \
    dummyimplugin.json \

target.path += $$MALIIT_TEST_LIBDIR/plugins

INVALIDPLUGINS = ../plugins/libinvalidplugin.so
invalidplugins.path = $$MALIIT_TEST_LIBDIR/plugins
invalidplugins.files = $$INVALIDPLUGINS

INSTALLS += target invalidplugins

QMAKE_CLEAN += ../plugins/libdummyimplugin.so

QMAKE_EXTRA_TARGETS += check
check.target = check
check.command = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.command = $$system(true)

QMAKE_EXTRA_TARGETS += memcheck
memcheck.target = memcheck
memcheck.command = $$system(true)
