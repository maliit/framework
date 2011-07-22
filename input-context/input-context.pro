include(../config.pri)

TEMPLATE = lib
TARGET = minputcontext
DEPENDPATH += .
INCLUDEPATH += . .. ../src

OBJECTS_DIR = .obj
MOC_DIR = .moc

external-libmaliit {
    PKGCONFIG += maliit-1.0
} else {
    INCLUDEPATH += ../maliit
    LIBS += ../maliit/lib$${MALIIT_LIB}.so
}

nomeegotouch {
    DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"Maliit\\\" 
} else {
    DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"MInputContext\\\" 

    CONFIG  += meegotouch
    STYLE_HEADERS += \
        mpreeditstyle.h \
}

# Input
HEADERS += minputcontext.h \
    minputcontextplugin.h \
    mdbusglibinputcontextadaptor.h \
    glibdbusimserverproxy.h \
    $$STYLE_HEADERS \

SOURCES += minputcontext.cpp \
    minputcontextplugin.cpp \
    mdbusglibinputcontextadaptor.cpp \
    glibdbusimserverproxy.cpp \

QT = core gui
CONFIG += plugin link_pkgconfig

PKGCONFIG += dbus-glib-1

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda

target.path += $$[QT_INSTALL_PLUGINS]/inputmethods
INSTALLS    += target \

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += lib$${TARGET}.so

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so

QMAKE_EXTRA_TARGETS += mdbusglibicconnectionserviceglue.h
mdbusglibicconnectionserviceglue.h.commands = \
    dbus-binding-tool --prefix=m_dbus_glib_input_context_adaptor --mode=glib-server \
        --output=mdbusglibicconnectionserviceglue.h minputmethodcontext1interface.xml
mdbusglibicconnectionserviceglue.h.depends = minputmethodcontext1interface.xml
