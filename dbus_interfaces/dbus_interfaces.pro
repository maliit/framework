TOP_DIR = ..

include($$TOP_DIR/config.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = dummy

OTHER_FILES += \
    minputmethodcontext1interface.xml \
    minputmethodserver1interface.xml

glib {
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
}
