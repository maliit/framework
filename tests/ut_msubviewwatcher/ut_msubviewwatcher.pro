include(../common_top.pri)

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_msubviewwatcher.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \

SOURCES += \
    ut_msubviewwatcher.cpp \
    ../stubs/fakegconf.cpp \

CONFIG += plugin qdbus link_pkgconfig
PKGCONFIG += dbus-glib-1 dbus-1 gconf-2.0

LIBS += \
        $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \

target.files += \
    $$TARGET \

include(../common_check.pri)
