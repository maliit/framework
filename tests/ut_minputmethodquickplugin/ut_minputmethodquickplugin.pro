include(../common_top.pri)

QUICK_DIR = ../../input-method-quick
INCLUDEPATH += \
    $$SRC_DIR \
    $$QUICK_DIR \

QT += core gui declarative

IN_TREE_TEST_PLUGIN_DIR = $$OUT_PWD/../../examples/plugins
DEFINES += IN_TREE_TEST_PLUGIN_DIR=\\\"$${IN_TREE_TEST_PLUGIN_DIR}\\\"

# Input
HEADERS += \
    ut_minputmethodquickplugin.h \

SOURCES += \
    ut_minputmethodquickplugin.cpp \

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \
    $$QUICK_DIR/lib$${MALIIT_PLUGINS_QUICK_LIB}.so \

include(../common_check.pri)
