include(../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = $$MALIIT_PLUGINS_LIB
INCLUDEPATH += .. ../passthroughserver

# Input
HEADERSINSTALL = \
        minputmethodplugin.h \
        mabstractinputmethod.h \
        mabstractinputmethodhost.h \
        mimpluginmanager.h \
        mtoolbaritem.h \
        mtoolbardata.h \
        mkeyoverride.h \
        mkeyoverridedata.h \
        mattributeextension.h \
        minputmethodnamespace.h \
        mabstractinputmethodsettings.h \
        mtoolbarlayout.h \
        mimextensionevent.h \
        mimgraphicsview.h \
        mimwidget.h \
        mimplugindescription.h \
        mimsettings.h \

HEADERS += $$HEADERSINSTALL \
        mimpluginmanager_p.h \
        mimpluginmanageradaptor.h \
        mindicatorserviceclient.h \
        mimapplication.h \
        minputcontextconnection.h \
        minputmethodhost.h \
        mtoolbardata_p.h \
        mtoolbaritem_p.h \
        mkeyoverride_p.h \
        mattributeextensionmanager.h \
        mattributeextensionid.h \
        mtoolbarlayout_p.h \
        minputcontextglibdbusconnection.h \
        mimremotewindow.h \
        mimxerrortrap.h \
        mimxextension.h \
        mimhwkeyboardtracker.h \
        mimgraphicsview_p.h \
        mimwidget_p.h \
        ../passthroughserver/mpassthruwindow.h \
        mimrotationanimation.h \
        mimpluginsproxywidget.h \
        mimonscreenplugins.h \
        mimhwkeyboardtracker_p.h \

SOURCES += mimpluginmanager.cpp \
        mimpluginmanageradaptor.cpp \
        mindicatorserviceclient.cpp \
        mabstractinputmethod.cpp \
        mabstractinputmethodhost.cpp \
        minputmethodhost.cpp \
        minputcontextconnection.cpp \
        mimapplication.cpp \
        mtoolbaritem.cpp \
        mtoolbardata.cpp \
        mkeyoverride.cpp \
        mkeyoverridedata.cpp \
        mattributeextensionmanager.cpp \
        mattributeextensionid.cpp \
        mattributeextension.cpp \
        mtoolbarlayout.cpp \
        minputcontextglibdbusconnection.cpp \
        mimremotewindow.cpp \
        mimxerrortrap.cpp \
        mimxextension.cpp \
        mimextensionevent.cpp \
        mimsettings.cpp \
        mimhwkeyboardtracker.cpp \
        mimgraphicsview.cpp \
        mimwidget.cpp \
        ../passthroughserver/mpassthruwindow.cpp \
        mimrotationanimation.cpp \
        mimplugindescription.cpp \
        mimpluginsproxywidget.cpp \
        mimonscreenplugins.cpp \

CONFIG += qdbus link_pkgconfig
QT = core gui xml

PKGCONFIG += dbus-glib-1 dbus-1 gconf-2.0

enable-contextkit {
    PKGCONFIG += contextsubscriber-1.0
    DEFINES += HAVE_CONTEXTSUBSCRIBER
}

contains(DEFINES, HAVE_MEEGOGRAPHICSSYSTEM) {
    QT += meegographicssystemhelper
}

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

target.path += $$M_IM_INSTALL_LIBS

OBJECTS_DIR = .obj
MOC_DIR = .moc

headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_PLUGINS_HEADER
headers.files += $$HEADERSINSTALL

contains(DEFINES, M_IM_DISABLE_TRANSLUCENCY) {
    M_IM_FRAMEWORK_FEATURE += M_IM_DISABLE_TRANSLUCENCY
} else {
    M_IM_FRAMEWORK_FEATURE -= M_IM_DISABLE_TRANSLUCENCY
}

nomeegotouch {
    outputFiles(maliit-plugins-$${MALIIT_PLUGINS_INTERFACE_VERSION}.pc, maliit-framework.schemas)
} else {
    outputFiles(MeegoImFramework.pc, meegoimframework.prf, meego-im-framework.schemas)
}

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = MeegoImFramework.pc maliit-plugins-$${MALIIT_PLUGINS_INTERFACE_VERSION}.pc

install_prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
install_prf.files = meegoimframework.prf

install_schemas.path = $$M_IM_INSTALL_SCHEMAS
install_schemas.files = meego-im-framework.schemas maliit-framework.schemas

INSTALLS += target \
    headers \
    install_prf \
    install_pkgconfig \
    install_schemas \

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += lib$${TARGET}.so.$${VERSION}

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so.$${VERSION}

LIBS += -lXcomposite -lXdamage -lX11 -lXfixes

QMAKE_EXTRA_TARGETS += mdbusglibicconnectionserviceglue.h
mdbusglibicconnectionserviceglue.h.commands = \
    dbus-binding-tool --prefix=m_dbus_glib_ic_connection --mode=glib-server \
        --output=mdbusglibicconnectionserviceglue.h minputmethodserver1interface.xml
mdbusglibicconnectionserviceglue.h.depends = minputmethodserver1interface.xml
