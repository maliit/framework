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
    $$SRC_DIR/libmeegoimframework.so.0 \

include(../common_check.pri)
