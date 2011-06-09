include(../config.pri)

TEMPLATE = lib
TARGET = minputcontext
DEPENDPATH += .
INCLUDEPATH += . ../src

OBJECTS_DIR = .obj
MOC_DIR = .moc

nomeegotouch {
    INCLUDEPATH += ../maliit
    LIBS += ../maliit/libmaliit.so
} else {
    CONFIG  += meegotouch
    DEFINES += HAVE_MEEGOTOUCH

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

QMAKE_CLEAN += *.gcno *.gcda

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
