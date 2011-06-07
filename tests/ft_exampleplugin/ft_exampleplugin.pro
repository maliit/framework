include(../common_top.pri)

IN_TREE_TEST_PLUGIN_DIR = $$OUT_PWD/../../examples/plugins/standard
DEFINES += IN_TREE_TEST_PLUGIN_DIR=\\\"$${IN_TREE_TEST_PLUGIN_DIR}\\\"

INCLUDEPATH += \
    $$SRC_DIR \
    
DEFINES += UNIT_TEST
QT += core gui

# Input
HEADERS += \
    ft_exampleplugin.h \

SOURCES += \
    ft_exampleplugin.cpp \

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \

include(../common_check.pri)
