include(../common_top.pri)

QT += quick

# For MImInputContextConnection pulled in by TestInputMethodHost
include($$TOP_DIR/connection/libmaliit-connection.pri)

IN_TREE_TEST_PLUGIN_DIR = $${OUT_PWD}/../../examples/plugins
DEFINES += IN_TREE_TEST_PLUGIN_DIR=\\\"$${IN_TREE_TEST_PLUGIN_DIR}\\\"

# Input
HEADERS += \
    ut_minputmethodquickplugin.h \
    ../utils/gui-utils.h \

SOURCES += \
    ut_minputmethodquickplugin.cpp \
    ../utils/gui-utils.cpp \

include($$TOP_DIR/src/libmaliit-plugins.pri)
include($$TOP_DIR/maliit-plugins-quick/libmaliit-plugins-quick.pri)
include($$TOP_DIR/maliit-plugins-quick/plugin-factory/libmaliit-plugins-quick-factory.pri)

include(../common_check.pri)
