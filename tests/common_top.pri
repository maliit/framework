include(../config.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
UTILS_DIR = ../utils

CONFIG += debug
QT += testlib xml

QMAKE_CLEAN += *.log *~

target.path = $$MALIIT_TEST_LIBDIR/$$TARGET
INSTALLS += target

TOP_DIR = ../..
SRC_DIR = $$TOP_DIR/src
PASSTHROUGH_DIR = $$TOP_DIR/passthroughserver

INCLUDEPATH += . \
               $$TOP_DIR \
               $$SRC_DIR \
               $$TOP_DIR/common \
               $$PASSTHROUGH_DIR \
               $$UTILS_DIR \

HEADERS += \
           $$UTILS_DIR/utils.h \

SOURCES += \
           $$UTILS_DIR/utils.cpp \


code_coverage_option=off
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        code_coverage_option=on
    }
}
