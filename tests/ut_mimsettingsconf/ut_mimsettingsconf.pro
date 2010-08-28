OBJECTS_DIR = .obj
MOC_DIR = .moc

SRC_DIR = ../../src
SETTINGSAPPLET_DIR = ../../settings-applet

INCLUDEPATH += $$SRC_DIR \
               $$SETTINGSAPPLET_DIR \
               ../stubs \

# Input
HEADERS += \
    ut_mimsettingsconf.h \
    ../stubs/mgconfitem_stub.h \
    ../stubs/fakegconf.h \
    $$SETTINGSAPPLET_DIR/mimsettingsconf.h \

SOURCES += \
    ut_mimsettingsconf.cpp \
    ../stubs/fakegconf.cpp \
    $$SETTINGSAPPLET_DIR/mimsettingsconf.cpp \


CONFIG += debug plugin meegotouch qdbus

LIBS += \
    ../../src/libmeegoimframework.so.0 \

include(../common_check.pri)
