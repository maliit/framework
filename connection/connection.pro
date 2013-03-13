include(../config.pri)

TOP_DIR = ..

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$$MALIIT_CONNECTION_LIB

include($$TOP_DIR/common/libmaliit-common.pri)

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
    load(wayland-scanner)
    PUBLIC_SOURCES += \
        minputcontextwestonimprotocolconnection.cpp
    PUBLIC_HEADERS += \
        minputcontextwestonimprotocolconnection.h
    WAYLANDSOURCES += \
        $$IN_PWD/input-method.xml \
        $$IN_PWD/text.xml
    CONFIG += link_pkgconfig
    PKGCONFIG += wayland-client
}

include($$TOP_DIR/dbus_interfaces/dbus_interfaces.pri)

qdbus-dbus-connection {
    server_adaptor.files = $$DBUS_SERVER_XML
    server_adaptor.header_flags = -i dbusinputcontextconnection.h -l DBusInputContextConnection
    server_adaptor.source_flags = -l DBusInputContextConnection

    DBUS_ADAPTORS = server_adaptor

    DBUS_INTERFACES = $$DBUS_CONTEXT_XML
    QDBUSXML2CPP_INTERFACE_HEADER_FLAGS = -i maliit/namespace.h -i maliit/settingdata.h

    PRIVATE_HEADERS += \
        dbuscustomarguments.h \
        \ # server
        dbusinputcontextconnection.h \

    PRIVATE_SOURCES += \
        dbuscustomarguments.cpp \
        \ # server
        dbusinputcontextconnection.cpp \

    CONFIG += dbus
}

qdbus-dbus-connection {
    QT += dbus

    !enable-dbus-activation {
        DEFINES += NO_DBUS_ACTIVATION
    }

    PRIVATE_HEADERS += \
        serverdbusaddress.h \

    PRIVATE_SOURCES += \
        serverdbusaddress.cpp \

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

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS \

SOURCES += \
    $$PUBLIC_SOURCES \
    $$PRIVATE_SOURCES \

target.path += $$LIBDIR

INSTALLS += target \

OTHER_FILES += libmaliit-connection.pri
