include(../config.pri)

TOP_DIR = ..

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$$MALIIT_PLUGINS_LIB

# Input
PLUGIN_HEADERS_PUBLIC = \
        maliit/plugins/inputmethodplugin.h \
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

PLUGIN_SOURCES += \
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

PLUGIN_HEADERS_PRIVATE += \
        maliit/plugins/keyoverride_p.h \
        maliit/plugins/attributeextension_p.h \
        maliit/plugins/extensionevent_p.h \
        maliit/plugins/updateevent_p.h \

SERVER_HEADERS_PUBLIC += \
        mimserver.h \
        mimabstractserverlogic.h \
        mimstandaloneserverlogic.h \

SERVER_SOURCES += \
        mimserver.cpp \
        mimabstractserverlogic.cpp \
        mimstandaloneserverlogic.cpp \

SERVER_HEADERS_PRIVATE += \
        mimpluginmanager.h \
        mimpluginmanager_p.h \
        minputmethodhost.h \
        mattributeextensionid.h \
        mattributeextensionmanager.h \
        msharedattributeextensionmanager.h \
        mimhwkeyboardtracker.h \
        mimonscreenplugins.h \
        mimhwkeyboardtracker_p.h \
        mimsubviewoverride.h \
        mimserveroptions.h \
        windowgroup.h \
        windowdata.h \
        abstractplatform.h \
        xcbplatform.h \
        unknownplatform.h \

SERVER_SOURCES += \
        mimpluginmanager.cpp \
        minputmethodhost.cpp \
        mattributeextensionid.cpp \
        mattributeextensionmanager.cpp \
        msharedattributeextensionmanager.cpp \
        mimhwkeyboardtracker.cpp \
        mimonscreenplugins.cpp \
        mimsubviewoverride.cpp \
        mimserveroptions.cpp \
        windowgroup.cpp \
        windowdata.cpp \
        abstractplatform.cpp \
        xcbplatform.cpp \
        unknownplatform.cpp \

wayland {
    SERVER_HEADERS_PRIVATE += waylandplatform.h
    SERVER_SOURCES += waylandplatform.cpp
}

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

CONFIG += link_pkgconfig
QT = core gui gui-private xml dbus qml quick

enable-contextkit {
    PKGCONFIG += contextsubscriber-1.0
    DEFINES += HAVE_CONTEXTSUBSCRIBER
} else {
    # libudev needed by non-contextkit MImHwKeyboardTracker
    PKGCONFIG += libudev
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

OTHER_FILES += \
    maliit-server.pc.in \
    maliit-plugins.pc.in \
    libmaliit-plugins.pri

OTHER_FILES += \
    config.h.in \
    maliit-plugins.prf.in \
    maliit-defines.prf.in \

outputFiles(config.h  maliit-defines.prf maliit-plugins.prf maliit-plugins.pc maliit-server.pc)

install_pkgconfig.path = $${LIBDIR}/pkgconfig
install_pkgconfig.files = \
    $$OUT_PWD/MeegoImFramework.pc \
    $$OUT_PWD/maliit-plugins.pc \
    $$OUT_PWD/maliit-server.pc \

install_prf.path = $$MALIIT_INSTALL_PRF
install_prf.files = $$OUT_PWD/maliit-plugins.prf $$OUT_PWD/maliit-defines.prf

INSTALLS += \
    target \
    plugins_headers \
    server_headers \
    install_prf \
    install_pkgconfig \

include($$TOP_DIR/maliit-plugins-quick/libmaliit-plugins-quick.pri)
include($$TOP_DIR/weston-protocols/libmaliit-weston-protocols.pri)
include($$TOP_DIR/connection/libmaliit-connection.pri)
include($$TOP_DIR/common/libmaliit-common.pri)
