include(../common_top.pri)

QT += $$QT_WIDGETS

INCLUDEPATH += ../stubs \
               ../dummyimplugin \
               ../dummyimplugin3 \

# Input
HEADERS += \
    ut_mimpluginmanager.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \

SOURCES += \
    ut_mimpluginmanager.cpp \
    ../stubs/fakegconf.cpp \

CONFIG += plugin qdbus link_pkgconfig

# For MImInputContextConnection pulled in by TestInputMethodHost
include(../../connection/libmaliit-connection.pri)

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \
    -L ../plugins/ \
    -ldummyimplugin \
    -ldummyimplugin3 \

target.files += \
    $$TARGET \
    toolbar1.xml \
    toolbar2.xml \

include(../common_check.pri)
