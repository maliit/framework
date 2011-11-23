include(../../config.pri)

TEMPLATE = lib
TARGET = maliit-im-common

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0 gthread-2.0 dbus-glib-1 gio-2.0

debug{
    DEFINES += ENABLE_DEBUG
}

# Generated dbus glue code has warnings of this type, so disable them
QMAKE_CFLAGS_DEBUG += -Wno-unused-parameter
QMAKE_CFLAGS_RELEASE += -Wno-unused-parameter

HEADERS += \
    meego-im-proxy.h \
    meego-imcontext-dbus.h \
    meego-im-connector.h \
    qt-keysym-map.h \
    debug.h \

SOURCES += \
    meego-im-proxy.c \
    meego-imcontext-dbus.c \
    meego-im-connector.c \
    qt-keysym-map.cpp \

GTK3_IM_MODULEDIR =

target.path = $$M_IM_INSTALL_LIBS

INSTALLS += target

EXTRA_FILES = \
    meego-im-client.xml \
    meego-imcontext-dbus.xml


# improxy
# Generate dbus glue
QMAKE_EXTRA_TARGETS += dbus_glue_improxy
dbus_glue_improxy.target = $$OUT_PWD/meego-im-proxy-glue.h
dbus_glue_improxy.output = $$OUT_PWD/meego-im-proxy-glue.h
dbus_glue_improxy.depends = $$IN_PWD/meego-im-client.xml
dbus_glue_improxy.commands = \
    dbus-binding-tool --prefix=meego_im_proxy --mode=glib-client \
        --output=$$OUT_PWD/meego-im-proxy-glue.h \
        $$IN_PWD/meego-im-client.xml

# Use to work around the fact that qmake looks up the target for the generated header wrong
QMAKE_EXTRA_TARGETS += fake_dbus_glue_improxy
fake_dbus_glue_improxy.target = meego-im-proxy-glue.h
fake_dbus_glue_improxy.depends = dbus_glue_improxy

# imcontext
# Generate dbus glue
QMAKE_EXTRA_TARGETS += dbus_glue_imcontext
dbus_glue_imcontext.target = $$OUT_PWD/meego-imcontext-dbus-glue.h
dbus_glue_imcontext.output = $$OUT_PWD/meego-imcontext-dbus-glue.h
dbus_glue_imcontext.depends = $$IN_PWD/meego-imcontext-dbus.xml
dbus_glue_imcontext.commands = \
    dbus-binding-tool --prefix=meego_imcontext_dbus --mode=glib-server \
        --output=$$OUT_PWD/meego-imcontext-dbus-glue.h \
        $$IN_PWD/meego-imcontext-dbus.xml

# Use to work around the fact that qmake looks up the target for the generated header wrong
QMAKE_EXTRA_TARGETS += fake_dbus_glue_imcontext
fake_dbus_glue_imcontext.target = meego-imcontext-dbus-glue.h
fake_dbus_glue_imcontext.depends = dbus_glue_imcontext
