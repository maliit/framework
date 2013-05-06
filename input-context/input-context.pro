include (../config.pri)

TOP_DIR = ..

QT += dbus gui-private quick
TEMPLATE = lib
CONFIG += plugin
TARGET = maliitplatforminputcontextplugin

target.path = $$[QT_INSTALL_PLUGINS]/platforminputcontexts

INSTALLS += target

include($$TOP_DIR/common/libmaliit-common.pri)
include($$TOP_DIR/connection/libmaliit-connection.pri)

SOURCES += $$PWD/main.cpp \
           $$PWD/minputcontext.cpp \


HEADERS += $$PWD/minputcontext.h \


OTHER_FILES += $$PWD/maliit.json
