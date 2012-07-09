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

include($$TOP_DIR/dbus_interfaces/dbus_interfaces.pri)

qdbus-dbus-connection {
    INTERFACE_LIST += $$DBUS_SERVER_XML $$DBUS_CONTEXT_XML
    SERVER_ADAPTOR_LIST += $$DBUS_SERVER_XML
    IC_ADAPTOR_LIST += $$DBUS_CONTEXT_XML

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

    qtPrepareTool(QMAKE_QDBUSXML2CPP, qdbusxml2cpp)

    server_adaptor_header.commands = $$QMAKE_QDBUSXML2CPP -i dbusinputcontextconnection.h -l DBusInputContextConnection -a ${QMAKE_FILE_OUT}: ${QMAKE_FILE_IN}
    server_adaptor_header.output_function = dbus_adaptor_header_output
    server_adaptor_header.name = DBUSXML2CPP ADAPTOR HEADER ${QMAKE_FILE_IN}
    server_adaptor_header.variable_out = DBUS_ADAPTOR_HEADERS
    server_adaptor_header.input = SERVER_ADAPTOR_LIST

    ic_adaptor_header.commands = $$QMAKE_QDBUSXML2CPP -i dbusserverconnection.h -l DBusServerConnection -a ${QMAKE_FILE_OUT}: ${QMAKE_FILE_IN}
    ic_adaptor_header.output_function = dbus_adaptor_header_output
    ic_adaptor_header.name = DBUSXML2CPP ADAPTOR HEADER ${QMAKE_FILE_IN}
    ic_adaptor_header.variable_out = DBUS_ADAPTOR_HEADERS
    ic_adaptor_header.input = IC_ADAPTOR_LIST

    defineReplace(dbus_adaptor_header_output) {
        return("$$lower($$section($$list($$basename(1)),.,-2,-2))_adaptor.h")
    }

    server_adaptor_source.commands = $$QMAKE_QDBUSXML2CPP -i ${QMAKE_FILE_OUT_BASE}.h -l DBusInputContextConnection -a :${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
    server_adaptor_source.output_function = dbus_adaptor_source_output
    server_adaptor_source.name = DBUSXML2CPP ADAPTOR SOURCE ${QMAKE_FILE_IN}
    server_adaptor_source.variable_out = SOURCES
    server_adaptor_source.input = SERVER_ADAPTOR_LIST

    ic_adaptor_source.commands = $$QMAKE_QDBUSXML2CPP -i ${QMAKE_FILE_OUT_BASE}.h -l DBusServerConnection -a :${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
    ic_adaptor_source.output_function = dbus_adaptor_source_output
    ic_adaptor_source.name = DBUSXML2CPP ADAPTOR SOURCE ${QMAKE_FILE_IN}
    ic_adaptor_source.variable_out = SOURCES
    ic_adaptor_source.input = IC_ADAPTOR_LIST

    adaptor_moc.commands = $$moc_header.commands
    adaptor_moc.output = $$moc_header.output
    adaptor_moc.depends = $$dbus_adaptor_header.output
    adaptor_moc.input = DBUS_ADAPTOR_HEADERS
    adaptor_moc.variable_out = GENERATED_SOURCES
    adaptor_moc.name = $$moc_header.name

    defineReplace(dbus_adaptor_source_output) {
        return("$$lower($$section($$list($$basename(1)),.,-2,-2))_adaptor.cpp")
    }

    interface_header.commands = $$QMAKE_QDBUSXML2CPP -i maliit/namespace.h -i maliit/settingdata.h -p ${QMAKE_FILE_OUT}: ${QMAKE_FILE_IN}
    interface_header.output_function = dbus_interface_header_output
    interface_header.name = DBUSXML2CPP INTERFACE HEADER ${QMAKE_FILE_IN}
    interface_header.variable_out = DBUS_INTERFACE_HEADERS
    interface_header.input = INTERFACE_LIST

    defineReplace(dbus_interface_header_output) {
        return("$$lower($$section($$list($$basename(1)),.,-2,-2))_interface.h")
    }

    interface_source.commands = $$QMAKE_QDBUSXML2CPP -i ${QMAKE_FILE_OUT_BASE}.h -p :${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
    interface_source.output_function = dbus_interface_source_output
    interface_source.name = DBUSXML2CPP INTERFACE SOURCE ${QMAKE_FILE_IN}
    interface_source.variable_out = SOURCES
    interface_source.input = INTERFACE_LIST

    interface_moc.commands = $$moc_header.commands
    interface_moc.output = $$moc_header.output
    interface_moc.depends = $$dbus_interface_header.output
    interface_moc.input = DBUS_INTERFACE_HEADERS
    interface_moc.variable_out = GENERATED_SOURCES
    interface_moc.name = $$moc_header.name

    defineReplace(dbus_interface_source_output) {
        return("$$lower($$section($$list($$basename(1)),.,-2,-2))_interface.cpp")
    }

    QMAKE_EXTRA_COMPILERS += server_adaptor_header ic_adaptor_header server_adaptor_source ic_adaptor_source adaptor_moc interface_header interface_source interface_moc
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

OTHER_FILES += maliit-connection-$${MALIIT_CONNECTION_INTERFACE_VERSION}.pc.in
outputFiles(maliit-connection-$${MALIIT_CONNECTION_INTERFACE_VERSION}.pc)

install_pkgconfig.path = $${LIBDIR}/pkgconfig
install_pkgconfig.files = maliit-connection-$${MALIIT_CONNECTION_INTERFACE_VERSION}.pc

INSTALLS += target \
    public_headers \
    install_pkgconfig \

OTHER_FILES += libmaliit-connection.pri

