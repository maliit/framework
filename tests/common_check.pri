QMAKE_EXTRA_TARGETS += check
check.target = check
check.commands = TESTING_IN_SANDBOX=1 TESTPLUGIN_PATH=../plugins LD_LIBRARY_PATH=../../src:../../input-context/:../../passthroughserver/:../plugins:$(LD_LIBRARY_PATH) ./$$TARGET
check.depends += $$TARGET

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.commands = ../rt.sh $$TARGET
check-xml.depends += $$TARGET

QMAKE_CLEAN += *.log *~

target.path = /usr/lib/meego-im-framework-tests/$$TARGET
INSTALLS += target

INCLUDEPATH += . ../../src

QT += testlib xml
# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs
        LIBS += -lgcov
    }
}
QMAKE_CLEAN += *.gcno *.gcda
