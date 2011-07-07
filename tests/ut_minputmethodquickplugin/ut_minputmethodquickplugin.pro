include(../common_top.pri)

QUICK_DIR = ../../input-method-quick
INCLUDEPATH += \
    $$SRC_DIR \
    $$QUICK_DIR \
    
DEFINES += UNIT_TEST
QT += core gui declarative

TEST_HELLOWORLD_PLUGIN_DIR = $$OUT_PWD/../../examples/plugins/quick
TEST_HELLOWORLD_PLUGIN_PATH=$$TEST_HELLOWORLD_PLUGIN_DIR/libhelloworldplugin.so
DEFINES += TEST_HELLOWORLD_PLUGIN_PATH=\\\"$${TEST_HELLOWORLD_PLUGIN_PATH}\\\"

TEST_CYCLEKEYS_PLUGIN_DIR = $$OUT_PWD/../../examples/plugins/cyclekeys
TEST_CYCLEKEYS_PLUGIN_PATH=$$TEST_CYCLEKEYS_PLUGIN_DIR/libcyclekeys.so
DEFINES += TEST_CYCLEKEYS_PLUGIN_PATH=\\\"$${TEST_CYCLEKEYS_PLUGIN_PATH}\\\"

# Input
HEADERS += \
    ut_minputmethodquickplugin.h \
    $$SRC_DIR/mabstractinputmethod.h \
    $$SRC_DIR/mabstractinputmethodhost.h \
    $$SRC_DIR/minputmethodhost.h \
    $$QUICK_DIR/minputmethodquick.h \
    $$QUICK_DIR/minputmethodquickplugin.h \

SOURCES += \
    ut_minputmethodquickplugin.cpp \
    $$SRC_DIR/mabstractinputmethod.cpp \
    $$SRC_DIR/mabstractinputmethodhost.cpp \
    $$SRC_DIR/minputmethodhost.cpp \
    $$QUICK_DIR/minputmethodquick.cpp \
    $$QUICK_DIR/minputmethodquickplugin.cpp \

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \
    $$QUICK_DIR/lib$${MALIIT_PLUGINS_QUICK_LIB}.so \

include(../common_check.pri)
