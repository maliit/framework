include(../common_top.pri)

IN_TREE_TEST_PLUGIN_DIR = $$OUT_PWD/../../examples/plugins
DEFINES += IN_TREE_TEST_PLUGIN_DIR=\\\"$${IN_TREE_TEST_PLUGIN_DIR}\\\"

INCLUDEPATH += \
    $$SRC_DIR \

QT += gui

# For MImInputContextConnection pulled in by TestInputMethodHost
LIBS += ../../connection/libmaliit-connection.a
POST_TARGETDEPS += ../../connection/libmaliit-connection.a

# Input
HEADERS += \
    ft_exampleplugin.h \

SOURCES += \
    ft_exampleplugin.cpp \

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \

include(../common_check.pri)
