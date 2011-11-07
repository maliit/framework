include(../../../config.pri)

TEMPLATE = app
TARGET = maliit-exampleapp-gtk2
target.path = $$M_IM_INSTALL_BIN

CONFIG -= qt
CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0

SOURCES = exampleapp-gtk.c

INSTALLS += target
QMAKE_CLEAN += target
