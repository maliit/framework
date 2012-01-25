include(../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = maliit-connection

INCLUDEPATH += ../common
LIBS += ../common/libmaliit-common.a
POST_TARGETDEPS += ../common/libmaliit-common.a

CONFIG += link_pkgconfig staticlib

# Interface classes
HEADERS += \
    mimserverconnection.h \
    minputcontextconnection.h \

SOURCES += \
    mimserverconnection.cpp \
    minputcontextconnection.cpp \

# Default to using glib-dbus based connection
CONFIG += glib-dbus-connection

glib-dbus-connection {

    # Used for serverdbusaddress
    CONFIG += qdbus

    disable-dbus-activation {
        DEFINES += NO_DBUS_ACTIVATION
    }

    HEADERS += \
        \ # common
        variantmarshalling.h \
        \ # input-context
        mdbusglibinputcontextadaptor.h \
        glibdbusimserverproxy.h \
        inputcontextdbusaddress.h \
        \ # server
        minputcontextglibdbusconnection.h \
        serverdbusaddress.h \

    SOURCES += \
        \ # common
        variantmarshalling.cpp \
        \ # input-context
        mdbusglibinputcontextadaptor.cpp \
        glibdbusimserverproxy.cpp \
        inputcontextdbusaddress.cpp \
        \ # server
        minputcontextglibdbusconnection.cpp \
        serverdbusaddress.cpp \

    PKGCONFIG += dbus-glib-1 gio-2.0

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

    # DBus activation
    !enable-legacy:!disable-dbus-activation {

        outputFiles(org.maliit.server.service)

        DBUS_SERVICES_DIR = $$system(pkg-config --variable session_bus_services_dir dbus-1)
        DBUS_SERVICES_PREFIX = $$system(pkg-config --variable prefix dbus-1)
        enforce-install-prefix {
            DBUS_SERVICES_DIR = $$replace(DBUS_SERVICES_DIR, $$DBUS_SERVICES_PREFIX, $$M_IM_PREFIX)
        }

        install_services.path = $$DBUS_SERVICES_DIR
        install_services.files = org.maliit.server.service
    }

    !enable-legacy:!disable-dbus-activation {
        INSTALLS += install_services
    }

    # Legacy dbus service file
    # installing dbus service file needs to be explicitly enabled
    dbusservice {
       INSTALLS += servicefiles
    }

    SERVICE_FILE = minputmethodserver.service.in

    ACTIVATION_OR_REAL_INTERFACE = com.meego.inputmethod.uiserveractivation

    servicefilegenerator.output = minputmethodserver.service
    servicefilegenerator.input = SERVICE_FILE
    servicefilegenerator.commands += sed -e \"s:M_BIN_PATH:$$M_IM_INSTALL_BIN:g\" -e \"s:SOFTWARE:$$SOFTWARE:g\" -e \"s:TARGET:$$TARGETDEVICE:g\" -e \"s:ACTIVATION_OR_REAL_INTERFACE:$$ACTIVATION_OR_REAL_INTERFACE:g\" -e \"s:MALIIT_SERVER:$$MALIIT_SERVER:g\" ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
    servicefilegenerator.CONFIG = target_predeps no_link
    QMAKE_EXTRA_COMPILERS += servicefilegenerator

    DBUS_SERVICES_DIR = $$system(pkg-config --variable session_bus_services_dir dbus-1)
    DBUS_SERVICES_PREFIX = $$system(pkg-config --variable prefix dbus-1)
    enforce-install-prefix {
        DBUS_SERVICES_DIR = $$replace(DBUS_SERVICES_DIR, $$DBUS_SERVICES_PREFIX, $$M_IM_PREFIX)
    }
    servicefiles.path = $$DBUS_SERVICES_DIR
    servicefiles.files = minputmethodserver.service
    servicefiles.CONFIG += no_check_exist
}



