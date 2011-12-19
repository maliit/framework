include(../config.pri)

TEMPLATE = lib
TARGET = $$MALIIT_INPUTCONTEXT_TARGETNAME
DEPENDPATH += .
INCLUDEPATH += . .. ../common

OBJECTS_DIR = .obj
MOC_DIR = .moc

external-libmaliit {
    PKGCONFIG += maliit-1.0
} else {
    INCLUDEPATH += ../maliit
    LIBS += ../maliit/lib$${MALIIT_LIB}.so
}

disable-dbus-activation {
    DEFINES += NO_DBUS_ACTIVATION
}

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"

# Input
HEADERS += minputcontext.h \
    minputcontextplugin.h \
    mdbusglibinputcontextadaptor.h \
    glibdbusimserverproxy.h \
    mimserverconnection.h \
    inputcontextdbusaddress.h \

SOURCES += minputcontext.cpp \
    minputcontextplugin.cpp \
    mdbusglibinputcontextadaptor.cpp \
    glibdbusimserverproxy.cpp \
    mimserverconnection.cpp \
    inputcontextdbusaddress.cpp \

OTHER_FILES += minputmethodcontext1interface.xml

QT = core gui
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

# Generate dbus glue
QMAKE_EXTRA_TARGETS += dbus_glue
dbus_glue.target = $$OUT_PWD/mdbusglibicconnectionserviceglue.h
dbus_glue.commands = \
    dbus-binding-tool --prefix=m_dbus_glib_input_context_adaptor --mode=glib-server \
        --output=$$OUT_PWD/mdbusglibicconnectionserviceglue.h $$IN_PWD/minputmethodcontext1interface.xml
dbus_glue.output = $$OUT_PWD/mdbusglibicconnectionserviceglue.h
dbus_glue.depends = $$IN_PWD/minputmethodcontext1interface.xml

# Use to work around the fact that qmake looks up the target for the generated header wrong
QMAKE_EXTRA_TARGETS += fake_dbus_glue
fake_dbus_glue.target = mdbusglibicconnectionserviceglue.h
fake_dbus_glue.depends = dbus_glue

OTHER_FILES += minputmethodcontext1interface.xml
