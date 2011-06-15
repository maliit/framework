include(../../config.pri)

TEMPLATE = lib
TARGET = ../plugins/$$qtLibraryTarget(dummyimplugin)
DEPENDPATH += .
INCLUDEPATH += . ../../src

OBJECTS_DIR = .obj
MOC_DIR = .moc

CONFIG += debug plugin

HEADERS += \
    dummyimplugin.h \
    dummyinputmethod.h \

SOURCES += \
    dummyimplugin.cpp \
    dummyinputmethod.cpp \

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
