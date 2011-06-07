include(../common_top.pri)

QUICK_DIR = ../../input-method-quick
INCLUDEPATH += \
    $$SRC_DIR \
    $$QUICK_DIR \
    
DEFINES += UNIT_TEST
QT += core gui declarative

IN_TREE_TEST_PLUGIN_DIR = $$OUT_PWD/../../examples/plugins/quick
DEFINES += IN_TREE_TEST_PLUGIN_DIR=\\\"$${IN_TREE_TEST_PLUGIN_DIR}\\\"

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
