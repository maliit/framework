include(../config.pri)

TOP_DIR = ..

TEMPLATE = lib
TARGET = $$MALIIT_INPUTCONTEXT_TARGETNAME
DEPENDPATH += .
INCLUDEPATH += . .. ../common ../connection

OBJECTS_DIR = .obj
MOC_DIR = .moc

INCLUDEPATH += ../maliit
LIBS += ../maliit/lib$${MALIIT_LIB}.so

CONFIG += direct-connection

direct-connection {
    LIBS += ../src/lib$${MALIIT_PLUGINS_LIB}.so
    INCLUDEPATH += ../src
}

include($$TOP_DIR/connection/libmaliit-connection.pri)

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

# Input
HEADERS += minputcontext.h \
    minputcontextplugin.h \

SOURCES += minputcontext.cpp \
    minputcontextplugin.cpp \

QT = core gui dbus
CONFIG += plugin link_pkgconfig

PKGCONFIG += dbus-glib-1
system(pkg-config gio-2.0 --atleast-version 2.26) {
    DEFINES   += MALIIT_USE_GIO_API
    PKGCONFIG += gio-2.0
}

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda

QT_IM_PLUGIN_PATH = $$[QT_INSTALL_PLUGINS]/inputmethods
QT_PREFIX = $$[QT_INSTALL_PREFIX]
enforce-install-prefix {
    QT_IM_PLUGIN_PATH = $$replace(QT_IM_PLUGIN_PATH, $$QT_PREFIX, $$M_IM_PREFIX)
}
target.path += $$QT_IM_PLUGIN_PATH
INSTALLS    += target \

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += lib$${TARGET}.so

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so

