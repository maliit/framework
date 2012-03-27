include(../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = $$MALIIT_CONNECTION_LIB

INCLUDEPATH += ../common
LIBS += ../common/libmaliit-common.a
POST_TARGETDEPS += ../common/libmaliit-common.a

CONFIG += link_pkgconfig

# Interface classes
PUBLIC_HEADERS += \
    mimserverconnection.h \
    minputcontextconnection.h \

PUBLIC_SOURCES += \
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
!disable-dbus {
    CONFIG += glib-dbus-connection
}

glib-dbus-connection {
    QT += dbus

    disable-dbus-activation {
        DEFINES += NO_DBUS_ACTIVATION
    }

    PRIVATE_HEADERS += \
        \ # common
        variantmarshalling.h \
        \ # input-context
        glibdbusimserverproxy.h \
        mdbusglibinputcontextadaptor.h \
        glibdbusimserverproxy_p.h \
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
            --output=$$OUT_PWD/mdbusglibicconnectionserviceglue.h $$IN_PWD/minputmethodserver1interface.xml
    dbus_glue_server.output = $$OUT_PWD/mdbusglibicconnectionserviceglue.h
    dbus_glue_server.depends = $$IN_PWD/minputmethodserver1interface.xml

    # Use to work around the fact that qmake looks up the target for the generated header wrong
    QMAKE_EXTRA_TARGETS += fake_dbus_glue_server
    fake_dbus_glue_server.target = mdbusglibicconnectionserviceglue.h
    fake_dbus_glue_server.depends = dbus_glue_server

    OTHER_FILES += minputmethodserver1interface.xml


    # Generate dbus glue for input-context side
    QMAKE_EXTRA_TARGETS += dbus_glue_inputcontext
    dbus_glue_inputcontext.target = $$OUT_PWD/mdbusglibinputcontextadaptorglue.h
    dbus_glue_inputcontext.commands = \
        dbus-binding-tool --prefix=m_dbus_glib_input_context_adaptor --mode=glib-server \
            --output=$$OUT_PWD/mdbusglibinputcontextadaptorglue.h $$IN_PWD/minputmethodcontext1interface.xml
    dbus_glue_inputcontext.output = $$OUT_PWD/mdbusglibinputcontextadaptorglue.h
    dbus_glue_inputcontext.depends = $$IN_PWD/minputmethodcontext1interface.xml

    # Use to work around the fact that qmake looks up the target for the generated header wrong
    QMAKE_EXTRA_TARGETS += fake_dbus_glue_inputcontext
    fake_dbus_glue_inputcontext.target = mdbusglibinputcontextadaptorglue.h
    fake_dbus_glue_inputcontext.depends = dbus_glue_inputcontext

    OTHER_FILES += minputmethodcontext1interface.xml
}

glib-dbus-connection {
    QT += dbus

    disable-dbus-activation {
        DEFINES += NO_DBUS_ACTIVATION
    }

    PUBLIC_HEADERS += \
        connectionfactory.h \

    PUBLIC_SOURCES += \
        connectionfactory.cpp \

    PRIVATE_HEADERS += \
        inputcontextdbusaddress.h \
        serverdbusaddress.h \

    PRIVATE_SOURCES += \
        inputcontextdbusaddress.cpp \
        serverdbusaddress.cpp \

    # DBus activation
    !disable-dbus-activation {
        outputFiles(org.maliit.server.service)

        DBUS_SERVICES_DIR = $$system(pkg-config --variable session_bus_services_dir dbus-1)
        DBUS_SERVICES_PREFIX = $$system(pkg-config --variable prefix dbus-1)
        enforce-install-prefix {
            DBUS_SERVICES_DIR = $$replace(DBUS_SERVICES_DIR, $$DBUS_SERVICES_PREFIX, $$M_IM_PREFIX)
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

target.path += $$M_IM_INSTALL_LIBS

public_headers.path += $$M_IM_INSTALL_HEADERS/$$MALIIT_CONNECTION_HEADER
public_headers.files += $$PUBLIC_HEADERS

OTHER_FILES += maliit-connection-$${MALIIT_CONNECTION_INTERFACE_VERSION}.pc.in
outputFiles(maliit-connection-$${MALIIT_CONNECTION_INTERFACE_VERSION}.pc)

install_pkgconfig.path = $${M_IM_INSTALL_LIBS}/pkgconfig
install_pkgconfig.files = maliit-connection-$${MALIIT_CONNECTION_INTERFACE_VERSION}.pc

INSTALLS += target \
    public_headers \
    install_pkgconfig \

OTHER_FILES += libmaliit-connection.pri

