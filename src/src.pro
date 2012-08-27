include(../config.pri)

TOP_DIR = ..

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$$MALIIT_PLUGINS_LIB

# Input
PLUGIN_HEADERS_PUBLIC = \
        maliit/plugins/abstractsurface.h \
        maliit/plugins/abstractwidgetssurface.h \
        maliit/plugins/abstractsurfacefactory.h \
        maliit/plugins/inputmethodplugin.h \
        maliit/plugins/abstractpluginfactory.h \
        maliit/plugins/abstractinputmethod.h \
        maliit/plugins/abstractinputmethodhost.h \
        maliit/plugins/keyoverride.h \
        maliit/plugins/keyoverridedata.h \
        maliit/plugins/attributeextension.h \
        maliit/plugins/extensionevent.h \
        maliit/plugins/updateevent.h \
        maliit/plugins/updatereceiver.h \
        maliit/plugins/plugindescription.h \
        maliit/plugins/subviewdescription.h \
        maliit/plugins/abstractpluginsetting.h \
        maliit/plugins/testsurfacefactory.h \

PLUGIN_SOURCES += \
        maliit/plugins/abstractsurface.cpp \
        maliit/plugins/abstractwidgetssurface.cpp \
        maliit/plugins/abstractsurfacefactory.cpp \
        maliit/plugins/abstractpluginfactory.cpp \
        maliit/plugins/abstractinputmethod.cpp \
        maliit/plugins/abstractinputmethodhost.cpp \
        maliit/plugins/keyoverride.cpp \
        maliit/plugins/keyoverridedata.cpp \
        maliit/plugins/attributeextension.cpp \
        maliit/plugins/extensionevent.cpp \
        maliit/plugins/updateevent.cpp \
        maliit/plugins/updatereceiver.cpp \
        maliit/plugins/plugindescription.cpp \
        maliit/plugins/subviewdescription.cpp \
        maliit/plugins/testsurfacefactory.cpp \

PLUGIN_HEADERS_PRIVATE += \
        maliit/plugins/keyoverride_p.h \
        maliit/plugins/attributeextension_p.h \
        maliit/plugins/extensionevent_p.h \
        maliit/plugins/updateevent_p.h \

SERVER_HEADERS_PUBLIC += \
        mimserver.h \
        mimabstractserverlogic.h \
        mimapphostedserverlogic.h \
        mimstandaloneserverlogic.h \

SERVER_SOURCES += \
        mimserver.cpp \
        mimabstractserverlogic.cpp \
        mimapphostedserverlogic.cpp \
        mimstandaloneserverlogic.cpp \

!contains(QT_MAJOR_VERSION, 5) {
    SERVER_HEADERS_PUBLIC += \
        mimdummyinputcontext.h \

    SERVER_SOURCES += \
        mimdummyinputcontext.cpp \
}

SERVER_HEADERS_PRIVATE += \
        abstractsurfacegroup.h \
        abstractsurfacegroupfactory.h \
        windowedsurface.h \
        windowedsurfacegroup.h \
        mimpluginsproxywidget.h \
        mimpluginmanager.h \
        mimpluginmanager_p.h \
        minputmethodhost.h \
        mattributeextensionid.h \
        mattributeextensionmanager.h \
        msharedattributeextensionmanager.h \
        mimhwkeyboardtracker.h \
        mimonscreenplugins.h \
        mimhwkeyboardtracker_p.h \
        mindicatorserviceclient.h \
        mimsubviewoverride.h \
        mimserveroptions.h \

SERVER_SOURCES += \
        abstractsurfacegroup.cpp \
        abstractsurfacegroupfactory.cpp \
        windowedsurface.cpp \
        windowedsurfacegroup.cpp \
        mimpluginmanager.cpp \
        minputmethodhost.cpp \
        mattributeextensionid.cpp \
        mattributeextensionmanager.cpp \
        msharedattributeextensionmanager.cpp \
        mimhwkeyboardtracker.cpp \
        mimpluginsproxywidget.cpp \
        mimonscreenplugins.cpp \
        mindicatorserviceclient.cpp \
        mimsubviewoverride.cpp \
        mimserveroptions.cpp \

SETTINGS_HEADERS_PRIVATE += \
        mimsettingsqsettings.h \
        mimsettings.h \

SETTINGS_SOURCES += \
        mimsettings.cpp \
        mimsettingsqsettings.cpp \

HEADERS += \
        $$PLUGIN_HEADERS_PUBLIC \
        $$PLUGIN_HEADERS_PRIVATE \
        $$SERVER_HEADERS_PUBLIC \
        $$SERVER_HEADERS_PRIVATE \
        $$SETTINGS_HEADERS_PRIVATE \

SOURCES += \
        $$PLUGIN_SOURCES \
        $$SERVER_SOURCES \
        $$SETTINGS_SOURCES \

x11 {
    HEADERS += \
        mpassthruwindow.h \
        mimremotewindow.h \
        mimxerrortrap.h \
        mimxextension.h \
        mimrotationanimation.h \
        mimxapplication.h \
        mimxserverlogic.h \

    SOURCES += \
        mimremotewindow.cpp \
        mimxerrortrap.cpp \
        mimxextension.cpp \
        mpassthruwindow.cpp \
        mimrotationanimation.cpp \
        mimxapplication.cpp \
        mimxserverlogic.cpp \
}

CONFIG += link_pkgconfig
QT = core $$QT_WIDGETS xml

!disable-dbus {
    CONFIG += qdbus
}

enable-contextkit {
    PKGCONFIG += contextsubscriber-1.0
    DEFINES += HAVE_CONTEXTSUBSCRIBER
}

disable-background-translucency {
    DEFINES += DISABLE_TRANSLUCENT_BACKGROUND_HINT
}

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda

target.path += $$LIBDIR

plugins_headers.path += $$INCLUDEDIR/$$MALIIT_PLUGINS_HEADER/maliit/plugins
plugins_headers.files += $$PLUGIN_HEADERS_PUBLIC

server_headers.path += $$INCLUDEDIR/$$MALIIT_SERVER_HEADER
server_headers.files += $$SERVER_HEADERS_PUBLIC

outputFiles(maliit-plugins-$${MALIIT_PLUGINS_INTERFACE_VERSION}.pc)


OTHER_FILES += \
    maliit-server-$${MALIIT_SERVER_INTERFACE_VERSION}.pc.in \
    maliit-plugins-$${MALIIT_PLUGINS_INTERFACE_VERSION}.pc.in \
    libmaliit-plugins.pri

outputFiles(maliit-server-$${MALIIT_SERVER_INTERFACE_VERSION}.pc)

outputFiles(config.h)
outputFiles(maliit-plugins.prf maliit-defines.prf)

OTHER_FILES += \
    config.h.in \
    maliit-plugins.prf.in \
    maliit-defines.prf.in \

install_pkgconfig.path = $${LIBDIR}/pkgconfig
install_pkgconfig.files = \
    $$OUT_PWD/MeegoImFramework.pc \
    $$OUT_PWD/maliit-plugins-$${MALIIT_PLUGINS_INTERFACE_VERSION}.pc \
    $$OUT_PWD/maliit-server-$${MALIIT_SERVER_INTERFACE_VERSION}.pc \

install_prf.path = $$MALIIT_INSTALL_PRF
install_prf.files = $$OUT_PWD/maliit-plugins.prf $$OUT_PWD/maliit-defines.prf

INSTALLS += \
    target \
    plugins_headers \
    server_headers \
    install_prf \
    install_pkgconfig \

x11:LIBS += -lXcomposite -lXdamage -lXfixes

include($$TOP_DIR/connection/libmaliit-connection.pri)
include($$TOP_DIR/common/libmaliit-common.pri)
