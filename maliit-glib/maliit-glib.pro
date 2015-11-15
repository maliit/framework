TOP_DIR = ..

include(../config.pri)

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$${MALIIT_GLIB_LIB}

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0 gobject-2.0 gio-2.0 gio-unix-2.0

CONFIG -= qt

QMAKE_CXXFLAGS_DEBUG+=-Wno-error=deprecated-declarations
QMAKE_CFLAGS_DEBUG+=-Wno-error=deprecated-declarations

HEADERSINSTALL += \
    maliitattributeextension.h \
    maliitattributeextensionregistry.h \
    maliitinputmethod.h \
    maliitsettingsmanager.h \
    maliitsettingsentry.h \
    maliitpluginsettings.h \
    maliitsettingdata.h \
    maliitbus.h \
    maliitserver.h \
    maliitcontext.h \

HEADERS += \
    $$HEADERSINSTALL \
    maliitattributeextensionprivate.h \
    maliitmarshallers.h \
    maliitpluginsettingsprivate.h \
    maliitsettingsentryprivate.h \

SOURCES += \
    maliitattributeextension.c \
    maliitattributeextensionregistry.c \
    maliitinputmethod.c \
    maliitsettingsmanager.c \
    maliitsettingsentry.c \
    maliitpluginsettings.c \
    maliitsettingdata.c \
    maliitbus.c \
    maliitserver.c \
    maliitcontext.c \

target.path += $$LIBDIR

headers.path += $$INCLUDEDIR/$$MALIIT_HEADER/maliit-glib
headers.files += $$HEADERSINSTALL

outputFiles(maliit-glib.pc)

OTHER_FILES += \
    maliit-glib.pc.in

install_pkgconfig.path = $${LIBDIR}/pkgconfig
install_pkgconfig.files = $$OUT_PWD/maliit-glib.pc

INSTALLS += \
    target \
    headers \
    install_pkgconfig \

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += \
    $$OBJECTS_DIR/*.gcno \
    $$OBJECTS_DIR/*.gcda \
    maliitmarshallers.h \
    maliitmarshallers.c

OTHER_FILES += \
    libmaliit-glib.pri

# generate marshallers
GLIB_GENMARSHAL_LIST += maliitmarshallers.list

OTHER_FILES += maliitmarshallers.list

glib_genmarshal_header.name = glib-genmarshal header ${QMAKE_FILE_IN}
glib_genmarshal_header.commands = glib-genmarshal --prefix=maliit_marshal --header --g-fatal-warnings ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
glib_genmarshal_header.output = ${QMAKE_FILE_IN_BASE}.h
glib_genmarshal_header.variable_out = HEADERS
glib_genmarshal_header.input = GLIB_GENMARSHAL_LIST

glib_genmarshal_source.name = glib-genmarshal source ${QMAKE_FILE_IN}
glib_genmarshal_source.commands = glib-genmarshal --prefix=maliit_marshal --body --g-fatal-warnings ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
glib_genmarshal_source.output = ${QMAKE_FILE_IN_BASE}.c
glib_genmarshal_source.variable_out = SOURCES
glib_genmarshal_source.input = GLIB_GENMARSHAL_LIST

QMAKE_EXTRA_COMPILERS += glib_genmarshal_header glib_genmarshal_source

include($$TOP_DIR/dbus_interfaces/dbus_interfaces.pri)

PRE_TARGETDEPS += glib_server glib_context
QMAKE_EXTRA_TARGETS += glib_server glib_context

glib_server.commands = gdbus-codegen --interface-prefix com.meego \
                                     --generate-c-code $$TOP_DIR/maliit-glib/maliitserver \
                                     --c-namespace Maliit \
                                     --annotate com.meego.inputmethod.uiserver1 org.gtk.GDBus.C.Name Server \
                                     $$DBUS_SERVER_XML

glib_context.commands = gdbus-codegen --interface-prefix com.meego \
                                      --generate-c-code $$TOP_DIR/maliit-glib/maliitcontext \
                                      --c-namespace Maliit \
                                      --annotate com.meego.inputmethod.inputcontext1 org.gtk.GDBus.C.Name Context \
                                      $$DBUS_CONTEXT_XML

