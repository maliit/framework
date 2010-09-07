
OBJECTS_DIR = .obj
MOC_DIR = .moc

QT += testlib xml

QMAKE_CLEAN += *.log *~

target.path = /usr/lib/meego-im-framework-tests/$$TARGET
INSTALLS += target

SRC_DIR = ../../src
PASSTHROUGH_DIR = ../../passthroughserver

INCLUDEPATH += . \
               $$SRC_DIR \
               $$PASSTHROUGH_DIR


code_coverage_option=off
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        code_coverage_option=on
    }
}
