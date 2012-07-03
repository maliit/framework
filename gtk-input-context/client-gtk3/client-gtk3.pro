TOP_DIR = ../..

include($$TOP_DIR/config.pri)

TEMPLATE = lib
TARGET = im-maliit
CONFIG += plugin

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-3.0

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

GTK3_IM_LIBDIR = $$system(pkg-config --variable=libdir gtk+-3.0)
GTK3_PREFIX = $$system(pkg-config --variable prefix gtk+-3.0)
GTK3_IM_LIBDIR = $$replace(GTK3_IM_LIBDIR, $$GTK3_PREFIX, $$PREFIX)

GTK3_BINARY_VERSION = $$system(pkg-config --variable=gtk_binary_version gtk+-3.0)
GTK3_DIR = $$GTK3_IM_LIBDIR/gtk-3.0/$$GTK3_BINARY_VERSION
GTK3_IM_MODULEDIR = $$GTK3_DIR/immodules

target.path += $$GTK3_IM_MODULEDIR

INSTALLS += target

!disable-gtk-cache-update {
    DISTRO = $$system(lsb_release -s -i)
    DISTRO_VERSION = $$system(lsb_release -s -r)

    isEqual(DISTRO, Ubuntu) {
        QUERY_IM_BIN = gtk-query-immodules-3.0
        greaterThan(DISTRO_VERSION, 11) {
            QUERY_IM_BIN = $$GTK3_IM_LIBDIR/libgtk-3-0/gtk-query-immodules-3.0
        }

        update-im-cache.path = $$GTK3_DIR/
        update-im-cache.extra = $$QUERY_IM_BIN > $$GTK3_DIR/gtk.immodules
        update-im-cache.uninstall = $$QUERY_IM_BIN > $$GTK3_DIR/gtk.immodules

        INSTALLS *= update-im-cache
    }

    system(test -e /etc/fedora-release) {
        update-im-cache.path = $$GTK3_DIR/
        64bit = $$find(GTK3_IM_LIBDIR, lib64)
        !isEmpty(64bit) {
            update-im-cache.extra = gtk-query-immodules-3.0-64 --update-cache
            update-im-cache.uninstall = gtk-query-immodules-3.0-64 --update-cache
        } else {
            update-im-cache.extra = gtk-query-immodules-3.0-32 --update-cache
            update-im-cache.uninstall = gtk-query-immodules-3.0-32 --update-cache
        }

        INSTALLS *= update-im-cache
    }
}
