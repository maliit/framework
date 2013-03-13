include(../config.pri)

TOP_DIR = ..

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$$MALIIT_CONNECTION_LIB

include($$TOP_DIR/common/libmaliit-common.pri)

DEFINES += MALIIT_INPUTCONTEXT_NAME=\\\"$${MALIIT_INPUTCONTEXT_NAME}\\\"
CONFIG += link_pkgconfig

# Interface classes
PUBLIC_HEADERS += \
    connectionfactory.h \
    mimserverconnection.h \
    minputcontextconnection.h \

PUBLIC_SOURCES += \
    connectionfactory.cpp \
    mimserverconnection.cpp \
    minputcontextconnection.cpp \

# Default to building direct connection
CONFIG += direct-connection

direct-connection {
    PUBLIC_SOURCES += \
        miminputcontextdirectconnection.cpp \
        mimdirectserverconnection.cpp \

    PUBLIC_HEADERS += \
        miminputcontextdirectconnection.h \
        mimdirectserverconnection.h \
}

# Default to building qdbus based connection
disable-dbus {
    DEFINES += MALIIT_DISABLE_DBUS
} else {
    CONFIG += qdbus-dbus-connection
}

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
    PKGCONFIG += wayland-client
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

    PRIVATE_HEADERS += \
        dbuscustomarguments.h \
        \ # input-context
        dbusserverconnection.h \
        \ # server
        dbusinputcontextconnection.h \

    PRIVATE_SOURCES += \
        dbuscustomarguments.cpp \
        \ # input-context
        dbusserverconnection.cpp \
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
        inputcontextdbusaddress.h \
        serverdbusaddress.h \

    PRIVATE_SOURCES += \
        inputcontextdbusaddress.cpp \
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

public_headers.path += $$INCLUDEDIR/$$MALIIT_CONNECTION_HEADER
public_headers.files += $$PUBLIC_HEADERS

OTHER_FILES += maliit-connection.pc.in
outputFiles(maliit-connection.pc)

install_pkgconfig.path = $${LIBDIR}/pkgconfig
install_pkgconfig.files = maliit-connection.pc

INSTALLS += target \
    public_headers \
    install_pkgconfig \

OTHER_FILES += libmaliit-connection.pri
