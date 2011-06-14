include(../common_top.pri)

SETTINGSAPPLET_DIR = ../../settings-applet

INCLUDEPATH += $$SRC_DIR \
               $$SETTINGSAPPLET_DIR \
               ../stubs \

# Input
HEADERS += \
    ut_mimsettingsconf.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \
    $$SETTINGSAPPLET_DIR/mimsettingsconf.h \

SOURCES += \
    ut_mimsettingsconf.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/minputcontextglibdbusconnection_serverinit_stub.cpp \
    $$SETTINGSAPPLET_DIR/mimsettingsconf.cpp \

DEFINES -= M_IM_PLUGINS_DIR=\\\"$$M_IM_PLUGINS_DIR\\\"
DEFINES += M_IM_PLUGINS_DIR=\\\"/usr/lib/meego-im-framework-tests/plugins\\\"

CONFIG += plugin qdbus link_pkgconfig
PKGCONFIG += dbus-glib-1 dbus-1 gconf-2.0

LIBS += \
    ../../src/libmeegoimframework.so.0 \

include(../common_check.pri)
