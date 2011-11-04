TEMPLATE = lib
TARGET = im-maliit

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0
PKGCONFIG += glib-2.0 gthread-2.0 dbus-glib-1 # FIXME: should not be needed

INCLUDEPATH = ../src

HEADERS += \
    ../client-gtk/client-imcontext-gtk.h \
    ../client-gtk/qt-gtk-translate.h \

SOURCES += \
    ../client-gtk/gtk-imcontext-plugin.c \
    ../client-gtk/client-imcontext-gtk.c \
    ../client-gtk/qt-gtk-translate.cpp \

LIBS += ../src/libmaliit-im-common.so

GTK2_IM_LIBDIR = $$system(pkg-config --variable=libdir gtk+-2.0)
GTK2_BINARY_VERSION = $$system(pkg-config --variable=gtk_binary_version gtk+-2.0)
GTK2_DIR = $$GTK2_IM_LIBDIR/gtk-2.0/$$GTK2_BINARY_VERSION
GTK2_IM_MODULEDIR = $$GTK2_DIR/immodules

target.path += $$GTK2_IM_MODULEDIR

INSTALLS += target

!disable-gtk-cache-update {
    DISTRO = $$system(lsb_release -s -i)
    isEqual(DISTRO, Ubuntu) {
        update-im-cache.path = $$GTK2_DIR/
        update-im-cache.extra = gtk-query-immodules-2.0 > $$GTK2_DIR/gtk.immodules

        INSTALLS *= update-im-cache
    }

    HOST = $$system(pkg-config --variable gtk_host gtk+-2.0)
    system(test -e /etc/fedora-release) {
        update-im-cache.path = $$GTK3_DIR/
        update-im-cache.extra = update-gtk-immodules $$HOST

        INSTALLS *= update-im-cache
    }
}
