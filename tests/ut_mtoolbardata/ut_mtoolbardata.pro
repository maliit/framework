include(../common_top.pri)

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mtoolbardata.h \
    ../stubs/mimsettings_stub.h \
    ../stubs/fakegconf.h \

SOURCES += \
    ut_mtoolbardata.cpp \
    ../stubs/fakegconf.cpp \

CONFIG += plugin qdbus

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \

target.files += \
    $$TARGET \
    toolbar1.xml \
    toolbar2.xml \
    toolbar3.xml \
    toolbar4.xml \

include(../common_check.pri)
