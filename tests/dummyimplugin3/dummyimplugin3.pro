TEMPLATE = lib
TARGET = ../plugins/$$qtLibraryTarget(dummyimplugin3)
DEPENDPATH += .
INCLUDEPATH += . ../../src
LIBS += -L../../src -lmeegoimframework

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CXXFLAGS += -Werror

CONFIG += plugin meegotouch

HEADERS += \
    dummyimplugin3.h \
    dummyinputmethod3.h \

SOURCES += \
    dummyimplugin3.cpp \
    dummyinputmethod3.cpp \

target.path += /usr/lib/meego-im-framework-tests/plugins

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
