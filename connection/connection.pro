include(../config.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET = maliit-connection

HEADERS += \
    variantmarshalling.h \

SOURCES += \
    variantmarshalling.cpp \


CONFIG += link_pkgconfig staticlib
PKGCONFIG += dbus-glib-1 gio-2.0
