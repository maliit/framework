include(../config.pri)

TOP_DIR = ..

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$$MALIIT_CONNECTION_LIB

include($$TOP_DIR/common/libmaliit-common.pri)
include($$TOP_DIR/weston-protocols/libmaliit-weston-protocols.pri)

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"
CONFIG += staticlib

# Interface classes
PUBLIC_HEADERS += \
    connectionfactory.h \
    minputcontextconnection.h \

PUBLIC_SOURCES += \
    connectionfactory.cpp \
    minputcontextconnection.cpp \

# Default to building qdbus based connection
CONFIG += qdbus-dbus-connection

wayland {
    QT += gui-private
    PUBLIC_SOURCES += \
        waylandinputmethodconnection.cpp
    PUBLIC_HEADERS += \
        waylandinputmethodconnection.h
}

include($$TOP_DIR/dbus_interfaces/dbus_interfaces.pri)

qdbus-dbus-connection {
    server_adaptor.files = $$DBUS_SERVER_XML
    server_adaptor.header_flags = -i dbusinputcontextconnection.h -l DBusInputContextConnection
    server_adaptor.source_flags = -l DBusInputContextConnection

    context_adaptor.files = $$DBUS_CONTEXT_XML
    context_adaptor.header_flags = -i dbusserverconnection.h -l DBusServerConnection
    context_adaptor.source_flags = -l DBusServerConnection

    DBUS_ADAPTORS = server_adaptor context_adaptor
    DBUS_INTERFACES = $$DBUS_SERVER_XML $$DBUS_CONTEXT_XML
    QDBUSXML2CPP_INTERFACE_HEADER_FLAGS = -i maliit/namespace.h -i maliit/settingdata.h

    QT += dbus

    !enable-dbus-activation {
        DEFINES += NO_DBUS_ACTIVATION
    }

    PRIVATE_HEADERS += \
        dbuscustomarguments.h \
        dbusinputcontextconnection.h \
        serverdbusaddress.h \
        mimserverconnection.h \
        dbusserverconnection.h \
        inputcontextdbusaddress.h \

    PRIVATE_SOURCES += \
        dbuscustomarguments.cpp \
        dbusinputcontextconnection.cpp \
        serverdbusaddress.cpp \
        mimserverconnection.cpp \
        dbusserverconnection.cpp \
        inputcontextdbusaddress.cpp \

    # DBus activation
    enable-dbus-activation {
        outputFiles(org.maliit.server.service)

        DBUS_SERVICES_DIR = $$system(pkg-config --variable session_bus_services_dir dbus-1)
        DBUS_SERVICES_PREFIX = $$system(pkg-config --variable prefix dbus-1)
        local-install {
            DBUS_SERVICES_DIR = $$replace(DBUS_SERVICES_DIR, $$DBUS_SERVICES_PREFIX, $$PREFIX)
        }

        install_services.path = $$DBUS_SERVICES_DIR
        install_services.files = org.maliit.server.service
        INSTALLS += install_services
    }
}

glib {
    include($$TOP_DIR/dbus_interfaces/dbus_interfaces.pri)

    PRE_TARGETDEPS += glib_server glib_context
    QMAKE_EXTRA_TARGETS += glib_server glib_context

    glib_server.commands = gdbus-codegen --interface-prefix com.meego \
                                         --generate-c-code maliitserver \
                                         --c-namespace Maliit \
                                         --annotate com.meego.inputmethod.uiserver1 org.gtk.GDBus.C.Name Server \
                                         $$DBUS_SERVER_XML

    glib_context.commands = gdbus-codegen --interface-prefix com.meego \
                                          --generate-c-code maliitcontext \
                                          --c-namespace Maliit \
                                          --annotate com.meego.inputmethod.inputcontext1 org.gtk.GDBus.C.Name Context \
                                          $$DBUS_CONTEXT_XML
}

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS \

SOURCES += \
    $$PUBLIC_SOURCES \
    $$PRIVATE_SOURCES \

OTHER_FILES += libmaliit-connection.pri
