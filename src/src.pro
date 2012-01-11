include(../config.pri)

TOP_DIR = ..

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$MALIIT_PLUGINS_LIB

# Input
PLUGIN_HEADERS_PUBLIC = \
        minputmethodplugin.h \
        mimabstractpluginfactory.h \
        mabstractinputmethod.h \
        mabstractinputmethodhost.h \
        mkeyoverride.h \
        mkeyoverridedata.h \
        mattributeextension.h \
        mimextensionevent.h \
        mimupdateevent.h \
        mimupdatereceiver.h \
        mimgraphicsview.h \
        mimwidget.h \
        mimplugindescription.h \
        mattributeextensionid.h \
        mimsubviewdescription.h \

PLUGIN_SOURCES += \
        mimabstractpluginfactory.cpp \
        mabstractinputmethod.cpp \
        mabstractinputmethodhost.cpp \
        mkeyoverride.cpp \
        mkeyoverridedata.cpp \
        mattributeextension.cpp \
        mimextensionevent.cpp \
        mimupdateevent.cpp \
        mimupdatereceiver.cpp \
        mimgraphicsview.cpp \
        mimwidget.cpp \
        mimplugindescription.cpp \
        mattributeextensionid.cpp \
        mimsubviewdescription.cpp \

PLUGIN_HEADERS_PRIVATE += \
        mkeyoverride_p.h \
        mimupdateevent_p.h \
        mimgraphicsview_p.h \
        mimwidget_p.h \
        mimextensionevent_p.h \

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
        mimpluginsproxywidget.h \
        mimpluginmanager.h \
        mimpluginmanager_p.h \
        minputmethodhost.h \
        mattributeextensionmanager.h \
        mimhwkeyboardtracker.h \
        mimonscreenplugins.h \
        mimhwkeyboardtracker_p.h \
        mindicatorserviceclient.h \
        mimsubviewoverride.h \
        mimserveroptions.h \

SERVER_SOURCES += \
        mimpluginmanager.cpp \
        minputmethodhost.cpp \
        mattributeextensionmanager.cpp \
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

!disable-gconf {
    SETTINGS_HEADERS_PRIVATE += \
        mimsettingsgconf.h \

    SETTINGS_SOURCES += \
        mimsettingsgconf.cpp \
}

disable-gconf {
    DEFINES += M_IM_DISABLE_GCONF
}

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

!disable-gconf {
    PKGCONFIG += gconf-2.0
}

!disable-dbus {
    CONFIG += qdbus
}

enable-contextkit {
    PKGCONFIG += contextsubscriber-1.0
    DEFINES += HAVE_CONTEXTSUBSCRIBER
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

target.path += $$M_IM_INSTALL_LIBS

plugins_headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_PLUGINS_HEADER
plugins_headers.files += $$PLUGIN_HEADERS_PUBLIC

server_headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_SERVER_HEADER
server_headers.files += $$SERVER_HEADERS_PUBLIC

outputFiles(maliit-plugins-$${MALIIT_PLUGINS_INTERFACE_VERSION}.pc, maliit-framework.schemas)


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

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = \
    $$OUT_PWD/MeegoImFramework.pc \
    $$OUT_PWD/maliit-plugins-$${MALIIT_PLUGINS_INTERFACE_VERSION}.pc \
    $$OUT_PWD/maliit-server-$${MALIIT_SERVER_INTERFACE_VERSION}.pc \

install_prf.path = $$M_IM_INSTALL_PRF
install_prf.files = $$OUT_PWD/maliit-plugins.prf $$OUT_PWD/maliit-defines.prf

install_schemas.files = $$OUT_PWD/maliit-framework.schemas
install_schemas.path = $$M_IM_INSTALL_SCHEMAS

INSTALLS += \
    target \
    plugins_headers \
    server_headers \
    install_prf \
    install_pkgconfig \

!disable-gconf {
    INSTALLS += install_schemas
}

# Registering the GConf schemas in the gconf database
!disable-gconf {
    gconftool = gconftool-2
    gconf_config_source = $$system(echo $GCONF_CONFIG_SOURCE)
    isEmpty(gconf_config_source) {
        gconf_config_source = $$system(gconftool-2 --get-default-source)
    }
}

QMAKE_EXTRA_TARGETS += register_schemas
register_schemas.target = register_schemas
register_schemas.commands += GCONF_CONFIG_SOURCE=$$gconf_config_source $$gconftool --makefile-install-rule $$install_schemas.files
install_schemas.depends += register_schemas

# Check targets
QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.depends += lib$${TARGET}.so.$${VERSION}

QMAKE_EXTRA_TARGETS += check
check.target = check
check.depends += lib$${TARGET}.so.$${VERSION}

x11:LIBS += -lXcomposite -lXdamage -lXfixes

include($$TOP_DIR/connection/libmaliit-connection.pri)
include($$TOP_DIR/common/libmaliit-common.pri)
