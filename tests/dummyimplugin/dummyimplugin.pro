TEMPLATE = lib
TARGET = ../plugins/$$qtLibraryTarget(dummyimplugin)
DEPENDPATH += .
INCLUDEPATH += . ../../src
LIBS += -L../../src -lmeegoimframework

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CXXFLAGS += -Werror

CONFIG += plugin meegotouch

HEADERS += \
    dummyimplugin.h \
    dummyinputmethod.h \

SOURCES += \
    dummyimplugin.cpp \
    dummyinputmethod.cpp \

target.path += /usr/lib/meego-im-framework-tests/plugins

INVALIDPLUGINS = ../plugins/libinvalidplugin.so
invalidplugins.path = /usr/lib/meego-im-framework-tests/plugins
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
