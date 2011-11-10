include(../../config.pri)

TEMPLATE = lib
TARGET = im-maliit

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-3.0
PKGCONFIG += glib-2.0 gthread-2.0 dbus-glib-1 # FIXME: should not be needed

debug{
    DEFINES += ENABLE_DEBUG
}

INCLUDEPATH = ../src

HEADERS += \
    ../client-gtk/client-imcontext-gtk.h \
    ../client-gtk/qt-gtk-translate.h \

SOURCES += \
    ../client-gtk/gtk-imcontext-plugin.c \
    ../client-gtk/client-imcontext-gtk.c \
    ../client-gtk/qt-gtk-translate.cpp \

LIBS += ../src/libmaliit-im-common.so

GTK3_IM_LIBDIR = $$system(pkg-config --variable=libdir gtk+-3.0)
GTK3_BINARY_VERSION = $$system(pkg-config --variable=gtk_binary_version gtk+-3.0)
GTK3_DIR = $$GTK3_IM_LIBDIR/gtk-3.0/$$GTK3_BINARY_VERSION
GTK3_IM_MODULEDIR = $$GTK3_DIR/immodules

target.path += $$GTK3_IM_MODULEDIR

INSTALLS += target

!disable-gtk-cache-update {
    DISTRO = $$system(lsb_release -s -i)
    isEqual(DISTRO, Ubuntu) {
        update-im-cache.path = $$GTK3_DIR/
        update-im-cache.extra = gtk-query-immodules-3.0 > $$GTK3_DIR/immodules.cache

        INSTALLS *= update-im-cache
    }

    system(test -e /etc/fedora-release) {
        update-im-cache.path = $$GTK3_DIR/
        64bit = $$find(GTK3_IM_LIBDIR, lib64)
        !isEmpty(64bit) {
            update-im-cache.extra = gtk-query-immodules-3.0-64 > $$GTK3_DIR/immodules.cache
        } else {
            update-im-cache.extra = gtk-query-immodules-3.0-32 > $$GTK3_DIR/immodules.cache
        }

        INSTALLS *= update-im-cache
    }
}
