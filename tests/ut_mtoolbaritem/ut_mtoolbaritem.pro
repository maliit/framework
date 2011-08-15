include(../common_top.pri)

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mtoolbaritem.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \

SOURCES += \
    ut_mtoolbaritem.cpp \
    ../stubs/fakegconf.cpp \

isEqual(code_coverage_option, off){
HEADERS += \
    $$SRC_DIR/mtoolbaritem.h \
    $$SRC_DIR/minputmethodnamespace.h \

SOURCES += \
    $$SRC_DIR/mtoolbaritem.cpp \
}

CONFIG += plugin qdbus

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \

include(../common_check.pri)
