TOP_DIR = ../..

include($$TOP_DIR/config.pri)

TEMPLATE = lib
TARGET = im-maliit
CONFIG += plugin

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-3.0

include($$TOP_DIR/maliit-glib/libmaliit-glib.pri)

DEFINES += G_LOG_DOMAIN=\\\"Maliit\\\"

INCLUDEPATH += ../src

HEADERS += \
    ../client-gtk/client-imcontext-gtk.h \
    ../client-gtk/qt-gtk-translate.h \

SOURCES += \
    ../client-gtk/gtk-imcontext-plugin.c \
    ../client-gtk/client-imcontext-gtk.c \
    ../client-gtk/qt-gtk-translate.cpp \

# DBus code
LIBS += ../src/libmaliit-gtk-im-common.a
PKGCONFIG += glib-2.0 gthread-2.0 dbus-glib-1 gio-2.0


GTK3_IM_LIBDIR = $$system(pkg-config --variable=libdir gtk+-3.0)
GTK3_PREFIX = $$system(pkg-config --variable prefix gtk+-3.0)
enforce-install-prefix {
    GTK3_IM_LIBDIR = $$replace(GTK3_IM_LIBDIR, $$GTK3_PREFIX, $$M_IM_PREFIX)
}

GTK3_BINARY_VERSION = $$system(pkg-config --variable=gtk_binary_version gtk+-3.0)
GTK3_DIR = $$GTK3_IM_LIBDIR/gtk-3.0/$$GTK3_BINARY_VERSION
GTK3_IM_MODULEDIR = $$GTK3_DIR/immodules

target.path += $$GTK3_IM_MODULEDIR

INSTALLS += target

!disable-gtk-cache-update {
    DISTRO = $$system(lsb_release -s -i)
    isEqual(DISTRO, Ubuntu) {
        update-im-cache.path = $$GTK3_DIR/
        update-im-cache.extra = gtk-query-immodules-3.0 --update-cache
        update-im-cache.uninstall = gtk-query-immodules-3.0 --update-cache

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
