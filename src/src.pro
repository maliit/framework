include(../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = $$MALIIT_PLUGINS_LIB
INCLUDEPATH += .. ../common

# Input
HEADERSINSTALL = \
        minputmethodplugin.h \
        mimabstractpluginfactory.h \
        mabstractinputmethod.h \
        mabstractinputmethodhost.h \
        mtoolbaritem.h \
        mtoolbardata.h \
        mkeyoverride.h \
        mkeyoverridedata.h \
        mattributeextension.h \
        mabstractinputmethodsettings.h \
        mtoolbarlayout.h \
        mimextensionevent.h \
        mimupdateevent.h \
        mimupdatereceiver.h \
        mimgraphicsview.h \
        mimwidget.h \
        mimplugindescription.h \
        mimsettings.h \
        mattributeextensionid.h \
        mimsubviewdescription.h \

HEADERS += \
        $$HEADERSINSTALL \
        mimpluginmanager.h \
        mimpluginmanager_p.h \
        mimpluginmanageradaptor.h \
        minputcontextconnection.h \
        minputmethodhost.h \
        mtoolbardata_p.h \
        mtoolbaritem_p.h \
        mkeyoverride_p.h \
        mattributeextensionmanager.h \
        mtoolbarlayout_p.h \
        minputcontextglibdbusconnection.h \
        mimhwkeyboardtracker.h \
        mimupdateevent_p.h \
        mimgraphicsview_p.h \
        mimwidget_p.h \
        mimpluginsproxywidget.h \
        mimonscreenplugins.h \
        mimhwkeyboardtracker_p.h \
        mimextensionevent_p.h \
        mimdummyinputcontext.h \
        mimserver.h \
        serverdbusaddress.h \
        mindicatorserviceclient.h \
        mimsubviewoverride.h \
        mtoolbaritemfilter.h \

SOURCES += \
        mimabstractpluginfactory.cpp \
        mimpluginmanager.cpp \
        mimpluginmanageradaptor.cpp \
        mabstractinputmethod.cpp \
        mabstractinputmethodhost.cpp \
        minputmethodhost.cpp \
        minputcontextconnection.cpp \
        mtoolbaritem.cpp \
        mtoolbardata.cpp \
        mkeyoverride.cpp \
        mkeyoverridedata.cpp \
        mattributeextensionmanager.cpp \
        mattributeextensionid.cpp \
        mattributeextension.cpp \
        mtoolbarlayout.cpp \
        minputcontextglibdbusconnection.cpp \
        mimextensionevent.cpp \
        mimupdateevent.cpp \
        mimupdatereceiver.cpp \
        mimsettings.cpp \
        mimhwkeyboardtracker.cpp \
        mimgraphicsview.cpp \
        mimwidget.cpp \
        mimplugindescription.cpp \
        mimpluginsproxywidget.cpp \
        mimonscreenplugins.cpp \
        mimsubviewdescription.cpp \
        mimdummyinputcontext.cpp \
        mimserver.cpp \
        serverdbusaddress.cpp \
        mindicatorserviceclient.cpp \
        mimsubviewoverride.cpp \
        mtoolbaritemfilter.cpp \

x11 {
    HEADERS += \
        mpassthruwindow.h \
        mimremotewindow.h \
        mimxerrortrap.h \
        mimxextension.h \
        mimrotationanimation.h \
        mimxapplication.h \

    SOURCES += \
        mimremotewindow.cpp \
        mimxerrortrap.cpp \
        mimxextension.cpp \
        mpassthruwindow.cpp \
        mimrotationanimation.cpp \
        mimxapplication.cpp \
}

qpa|qws {
    SOURCES += \
        mimqpaplatform.cpp \

    HEADERS += \
        mimqpaplatform.h
}

enable-legacy {
    HEADERS += \
        mimmeegoindicator.h \

    SOURCES += \
        mimmeegoindicator.cpp \
}

CONFIG += qdbus link_pkgconfig
QT = core $$QT_WIDGETS xml

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

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += $$OBJECTS_DIR/*.gcno $$OBJECTS_DIR/*.gcda

target.path += $$M_IM_INSTALL_LIBS

headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_PLUGINS_HEADER
headers.files += $$HEADERSINSTALL

contains(DEFINES, M_IM_DISABLE_TRANSLUCENCY) {
    M_IM_FRAMEWORK_FEATURE += M_IM_DISABLE_TRANSLUCENCY
} else {
    M_IM_FRAMEWORK_FEATURE -= M_IM_DISABLE_TRANSLUCENCY
}

!enable-legacy {
    outputFiles(maliit-plugins-$${MALIIT_PLUGINS_INTERFACE_VERSION}.pc, maliit-framework.schemas, org.maliit.server.service)
} else {
    outputFiles(MeegoImFramework.pc, meegoimframework.prf, meego-im-framework.schemas)
}

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/MeegoImFramework.pc $$OUT_PWD/maliit-plugins-$${MALIIT_PLUGINS_INTERFACE_VERSION}.pc

QT_PRF_DIR = $$[QT_INSTALL_DATA]/mkspecs/features
QT_PREFIX = $$[QT_INSTALL_PREFIX]
enforce-install-prefix {
    QT_PRF_DIR = $$replace(QT_PRF_DIR, $$QT_PREFIX, $$M_IM_PREFIX)
}

install_prf.path = $$QT_PRF_DIR
install_prf.files = $$OUT_PWD/meegoimframework.prf

!enable-legacy {
    install_schemas.files = $$OUT_PWD/maliit-framework.schemas
} else {
    install_schemas.files = $$OUT_PWD/meego-im-framework.schemas
}
install_schemas.path = $$M_IM_INSTALL_SCHEMAS

!enable-legacy:!disable-dbus-activation {
    DBUS_SERVICES_DIR = $$system(pkg-config --variable session_bus_services_dir dbus-1)
    DBUS_SERVICES_PREFIX = $$system(pkg-config --variable prefix dbus-1)
    enforce-install-prefix {
        DBUS_SERVICES_DIR = $$replace(DBUS_SERVICES_DIR, $$DBUS_SERVICES_PREFIX, $$M_IM_PREFIX)
    }

    install_services.path = $$DBUS_SERVICES_DIR
    install_services.files = org.maliit.server.service
}

INSTALLS += target \
    headers \
    install_prf \
    install_pkgconfig \
    install_schemas \

!enable-legacy:!disable-dbus-activation {
    INSTALLS += install_services
}

# Registering the GConf schemas in the gconf database
gconftool = gconftool-2
gconf_config_source = $$system(echo $GCONF_CONFIG_SOURCE)
isEmpty(gconf_config_source) {
    gconf_config_source = $$system(gconftool-2 --get-default-source)
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

# Generate dbus glue
QMAKE_EXTRA_TARGETS += dbus_glue
dbus_glue.target = $$OUT_PWD/mdbusglibicconnectionserviceglue.h
dbus_glue.commands = \
    dbus-binding-tool --prefix=m_dbus_glib_ic_connection --mode=glib-server \
        --output=$$OUT_PWD/mdbusglibicconnectionserviceglue.h $$IN_PWD/minputmethodserver1interface.xml
dbus_glue.output = $$OUT_PWD/mdbusglibicconnectionserviceglue.h
dbus_glue.depends = $$IN_PWD/minputmethodserver1interface.xml

# Use to work around the fact that qmake looks up the target for the generated header wrong
QMAKE_EXTRA_TARGETS += fake_dbus_glue
fake_dbus_glue.target = mdbusglibicconnectionserviceglue.h
fake_dbus_glue.depends = dbus_glue

OTHER_FILES += minputmethodserver1interface.xml
