include(../common_top.pri)

QUICK_DIR = ../../maliit-plugins-quick
QUICK_IM_DIR = $${QUICK_DIR}/input-method
QUICK_FACTORY_DIR = $${QUICK_DIR}/plugin-factory
INCLUDEPATH += \
    $$SRC_DIR \
    $$QUICK_IM_DIR \
    $$QUICK_FACTORY_DIR \

QT += $$QT_WIDGETS quick1

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

LIBS += \
    -L$${QUICK_IM_DIR} \
    -L$${QUICK_FACTORY_DIR} \
    -L$${SRC_DIR} \
    -l$${MALIIT_PLUGINS_LIB} \
    -l$${MALIIT_PLUGINS_QUICK_LIB} \
    -l$${MALIIT_PLUGINS_QUICK_FACTORY} \

include(../common_check.pri)
