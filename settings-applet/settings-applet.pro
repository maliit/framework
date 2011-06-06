include(../config.pri)

TEMPLATE      = lib
TARGET        = $$qtLibraryTarget(mimsettingsapplet)

include (desktop.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc

LIBS += ../src/lib$${MALIIT_PLUGINS_LIB}.so

CONFIG += ordered
CONFIG += plugin qdbus
QT = core gui

contains(CONFIG, nomeegotouch) {
} else {
    CONFIG  += meegotouch duicontrolpanel
    DEFINES += HAVE_MEEGOTOUCH
}

CONFIG += link_pkgconfig
PKGCONFIG += gconf-2.0

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
        CONFIG -= release
        CONFIG += debug
    }
}

QMAKE_CLEAN += *.gcno *.gcda

INCLUDEPATH += . ../src

HEADERS += \
    mimsettingsapplet.h \
    mimsettingsbrief.h \
    mimsettingsconf.h \
    mimsettingswidget.h \
    mimsettingslistitem.h \
    mimsubviewmodel.h

SOURCES += \
    mimsettingsapplet.cpp \
    mimsettingsbrief.cpp \
    mimsettingsconf.cpp \
    mimsettingswidget.cpp \
    mimsettingslistitem.cpp \
    mimsubviewmodel.cpp

target.path += $$DCP_APPLET_DIR

message ("Plugin path will be: "$$target.path)

INSTALLS += target

QMAKE_EXTRA_TARGETS += check-xml
check-xml.depends = lib$${TARGET}.so
check-xml.commands = $$system(true)

QMAKE_EXTRA_TARGETS += check
check.depends = lib$${TARGET}.so
check.commands = $$system(true)

# for check:
QMAKE_EXTRA_TARGETS += check
check.commands = $$system(true)
check.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check-xml
check-xml.commands = $$system(true)
check-xml.CONFIG = recursive
