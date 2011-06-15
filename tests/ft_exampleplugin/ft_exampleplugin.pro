include(../common_top.pri)

TEST_PLUGIN_DIR = $$OUT_PWD/../../examples/plugins/standard
TEST_PLUGIN_PATH=$$TEST_PLUGIN_DIR/libexampleplugin.so
DEFINES += TEST_PLUGIN_PATH=\\\"$${TEST_PLUGIN_PATH}\\\"

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
