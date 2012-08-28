TOP_DIR = ../..

include($$TOP_DIR/config.pri)

TEMPLATE = lib
TARGET = im-maliit
CONFIG += plugin

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0

include($$TOP_DIR/maliit-glib/libmaliit-glib.pri)
include($$TOP_DIR/connection-glib/libmaliit-connection-glib.pri)

DEFINES += G_LOG_DOMAIN=\\\"Maliit\\\"

HEADERS += \
    ../client-gtk/client-imcontext-gtk.h \
    ../client-gtk/qt-gtk-translate.h \

SOURCES += \
    ../client-gtk/gtk-imcontext-plugin.c \
    ../client-gtk/client-imcontext-gtk.c \
    ../client-gtk/qt-gtk-translate.cpp \

GTK2_IM_LIBDIR = $$system(pkg-config --variable=libdir gtk+-2.0)
GTK2_PREFIX = $$system(pkg-config --variable prefix gtk+-2.0)
local-install {
    GTK2_IM_LIBDIR = $$replace(GTK2_IM_LIBDIR, $$GTK2_PREFIX, $$PREFIX)
}

GTK2_BINARY_VERSION = $$system(pkg-config --variable=gtk_binary_version gtk+-2.0)
GTK2_DIR = $$GTK2_IM_LIBDIR/gtk-2.0/$$GTK2_BINARY_VERSION
GTK2_IM_MODULEDIR = $$GTK2_DIR/immodules

target.path += $$GTK2_IM_MODULEDIR

INSTALLS += target

!disable-gtk-cache-update {
    # need to make sure dynamic linker can find maliit libraries when running gtk-query-module
    ldconfig.extra = ldconfig
    ldconfig.path = . # dummy path
    INSTALLS += ldconfig

    DISTRO = $$system(lsb_release -s -i)
    DISTRO_VERSION = $$system(lsb_release -s -r)

    isEqual(DISTRO, Ubuntu) {
        QUERY_IM_BIN = gtk-query-immodules-2.0

        greaterThan(DISTRO_VERSION, 11) {
            QUERY_IM_BIN = $$GTK2_IM_LIBDIR/libgtk2.0-0/gtk-query-immodules-2.0
        }

        update-im-cache.path = $$GTK2_DIR/
        update-im-cache.extra = $$QUERY_IM_BIN > $$GTK2_DIR/gtk.immodules
        update-im-cache.uninstall = $$QUERY_IM_BIN > $$GTK2_DIR/gtk.immodules

        INSTALLS *= update-im-cache
    }

    HOST = $$system(pkg-config --variable gtk_host gtk+-2.0)
    system(test -e /etc/fedora-release) {
        update-im-cache.path = $$GTK2_DIR/
        update-im-cache.extra = update-gtk-immodules $$HOST
        update-im-cache.uninstall = update-gtk-immodules $$HOST

        INSTALLS *= update-im-cache
    }
}
