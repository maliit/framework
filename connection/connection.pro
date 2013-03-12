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

# Default to building glib-dbus based connection
disable-dbus {
    DEFINES += MALIIT_DISABLE_DBUS
} else {
    enable-qdbus {
        CONFIG += qdbus-dbus-connection
    } else {
        CONFIG += glib-dbus-connection
    }
}

contains(QT_MAJOR_VERSION, 5) {
    wayland {
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

glib-dbus-connection {
    DEFINES += HAVE_GLIB_DBUS

    PRIVATE_HEADERS += \
        \ # common
        variantmarshalling.h \
        \ # input-context
        glibdbusimserverproxy.h \
        glibdbusimserverproxy_p.h \
        mdbusglibinputcontextadaptor.h \
        \ # server
        minputcontextglibdbusconnection.h \

    PRIVATE_SOURCES += \
        \ # common
        variantmarshalling.cpp \
        \ # input-context
        glibdbusimserverproxy.cpp \
        mdbusglibinputcontextadaptor.cpp \
        \ # server
        minputcontextglibdbusconnection.cpp \

    PKGCONFIG += dbus-glib-1

    # Generate dbus glue for server side
    QMAKE_EXTRA_TARGETS += dbus_glue_server
    dbus_glue_server.target = $$OUT_PWD/mdbusglibicconnectionserviceglue.h
    dbus_glue_server.commands = \
        dbus-binding-tool --prefix=m_dbus_glib_ic_connection --mode=glib-server \
            --output=$$OUT_PWD/mdbusglibicconnectionserviceglue.h $$DBUS_SERVER_XML
    dbus_glue_server.output = $$OUT_PWD/mdbusglibicconnectionserviceglue.h
    dbus_glue_server.depends = $$DBUS_SERVER_XML

    # Use to work around the fact that qmake looks up the target for the generated header wrong
    QMAKE_EXTRA_TARGETS += fake_dbus_glue_server
    fake_dbus_glue_server.target = mdbusglibicconnectionserviceglue.h
    fake_dbus_glue_server.depends = dbus_glue_server

    # Generate dbus glue for input-context side
    QMAKE_EXTRA_TARGETS += dbus_glue_inputcontext
    dbus_glue_inputcontext.target = $$OUT_PWD/mdbusglibinputcontextadaptorglue.h
    dbus_glue_inputcontext.commands = \
        dbus-binding-tool --prefix=m_dbus_glib_input_context_adaptor --mode=glib-server \
            --output=$$OUT_PWD/mdbusglibinputcontextadaptorglue.h $$DBUS_CONTEXT_XML
    dbus_glue_inputcontext.output = $$OUT_PWD/mdbusglibinputcontextadaptorglue.h
    dbus_glue_inputcontext.depends = $$DBUS_CONTEXT_XML

    # Use to work around the fact that qmake looks up the target for the generated header wrong
    QMAKE_EXTRA_TARGETS += fake_dbus_glue_inputcontext
    fake_dbus_glue_inputcontext.target = mdbusglibinputcontextadaptorglue.h
    fake_dbus_glue_inputcontext.depends = dbus_glue_inputcontext

    GLIB_GENMARSHAL_LIST += maliitmarshalers.list

    OTHER_FILES += maliitmarshalers.list

    glib_genmarshal_header.name = glib-genmarshal header ${QMAKE_FILE_IN}
    glib_genmarshal_header.commands = glib-genmarshal --prefix=_maliit_marshal --header --g-fatal-warnings ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
    glib_genmarshal_header.output = ${QMAKE_FILE_IN_BASE}.h
    glib_genmarshal_header.variable_out = PRIVATE_HEADERS
    glib_genmarshal_header.input = GLIB_GENMARSHAL_LIST

    glib_genmarshal_source.name = glib-genmarshal source ${QMAKE_FILE_IN}
    glib_genmarshal_source.commands = glib-genmarshal --prefix=_maliit_marshal --body --g-fatal-warnings ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
    glib_genmarshal_source.output = ${QMAKE_FILE_IN_BASE}.c
    glib_genmarshal_source.variable_out = SOURCES
    glib_genmarshal_source.input = GLIB_GENMARSHAL_LIST

    QMAKE_EXTRA_COMPILERS += glib_genmarshal_header glib_genmarshal_source
}

qdbus-dbus-connection|glib-dbus-connection {
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
