OBJECTS_DIR = .obj
MOC_DIR = .moc

SRC_PATH = ../../passthroughserver
INCLUDEPATH += $$SRC_PATH

LIBS += ../../src/libmeegoimframework.so -lXfixes


# Input
HEADERS += \
    ut_passthroughserver.h \
    $$SRC_PATH/mpassthruwindow.h \

SOURCES += \
    ut_passthroughserver.cpp \
    $$SRC_PATH/mpassthruwindow.cpp \


CONFIG += debug meegotouch

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

include(../common_check.pri)
