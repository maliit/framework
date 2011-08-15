include(../common_top.pri)

INCLUDEPATH += ../stubs \
               ../dummyimplugin \

SRC_DIR = ../../src


# Input
HEADERS += \
    ft_mimpluginmanager.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \
    ../stubs/fakeproperty.h \
    ../stubs/mkeyboardstatetracker_stub.h \

SOURCES += \
    ft_mimpluginmanager.cpp \
    ../stubs/fakegconf.cpp \
    ../stubs/fakeproperty.cpp \

CONFIG += plugin qdbus link_pkgconfig
PKGCONFIG += dbus-glib-1 dbus-1 gconf-2.0

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \
    -L ../plugins/ \
    -ldummyimplugin \

include(../common_check.pri)
