TEMPLATE = lib
TARGET = minputcontext
DEPENDPATH += .
INCLUDEPATH += . ../src

OBJECTS_DIR = .obj
MOC_DIR = .moc

STYLE_HEADERS += \
    mpreeditstyle.h \

# Input
HEADERS += minputcontext.h \
    minputcontextplugin.h \
    $$STYLE_HEADERS \

SOURCES += minputcontext.cpp \
    minputcontextplugin.cpp \

QT = core gui
CONFIG += plugin debug meegotouch

contains(DEFINES, QT_DBUS) {
    SOURCES += minputcontextadaptor.cpp qtdbusimserverproxy.cpp
    HEADERS += qtdbusimserverproxy.h minputcontextadaptor.h
    CONFIG += qdbus
} else {
    CONFIG += link_pkgconfig
    PKGCONFIG += dbus-glib-1
    SOURCES += mdbusglibinputcontextadaptor.cpp glibdbusimserverproxy.cpp
    HEADERS += mdbusglibinputcontextadaptor.h glibdbusimserverproxy.h
}

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

target.path += $$[QT_INSTALL_PLUGINS]/inputmethods
INSTALLS    += target \

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += lib$${TARGET}.so

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so

