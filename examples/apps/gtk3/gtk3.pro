include(../../../config.pri)

TEMPLATE = app
TARGET = maliit-exampleapp-gtk3
target.path = $$M_IM_INSTALL_BIN

CONFIG -= qt
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-3.0

SOURCES = ../gtk2/exampleapp-gtk.c

INSTALLS += target
QMAKE_CLEAN += target
