include(../common_top.pri)

INCLUDEPATH += ../stubs \

# Input
HEADERS += \
    ut_mimserveroptions.h \
    ../stubs/fakegconf.h \
#    ../stubs/mimsettings_stub.h \

SOURCES += \
    ut_mimserveroptions.cpp \
    ../stubs/fakegconf.cpp \

#CONFIG += plugin qdbus

LIBS += \
    $$SRC_DIR/lib$${MALIIT_PLUGINS_LIB}.so \

include(../common_check.pri)
