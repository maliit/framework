include(../../config.pri)

TEMPLATE = lib
TARGET = im-maliit

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0
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

GTK2_IM_LIBDIR = $$system(pkg-config --variable=libdir gtk+-2.0)
GTK2_BINARY_VERSION = $$system(pkg-config --variable=gtk_binary_version gtk+-2.0)
GTK2_IM_MODULEDIR = $$GTK2_IM_LIBDIR/gtk-2.0/$$GTK2_BINARY_VERSION/immodules

target.path += $$GTK2_IM_MODULEDIR

INSTALLS += target
