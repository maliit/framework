include(../config.pri)

CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS = \
          ut_minputcontextplugin \
          ut_minputcontext \
          dummyimplugin \
          dummyimplugin2 \
          dummyimplugin3 \
          dummyplugin \
          ut_mimpluginmanager \
          ut_passthroughserver \
          ft_mimpluginmanager \
          ut_mtoolbardata \
          ut_mtoolbaritem \
          ut_mattributeextensionmanager \
          ut_mimapplication \
          ut_mkeyoverride \
          ut_selfcompositing \
          ut_mimrotationanimation \
          ut_minputmethodquickplugin \
          ft_exampleplugin \

outputFiles(runtests.sh, gen-tests-xml.sh)

target.commands += $$OUT_PWD/gen-tests-xml.sh > $$OUT_PWD/tests.xml
target.path = $$MALIIT_TEST_DATADIR
target.files += qtestlib2junitxml.xsl $$OUT_PWD/runtests.sh $$OUT_PWD/tests.xml
target.depends += $$OUT_PWD/gen-tests-xml.sh
target.CONFIG += no_check_exist
INSTALLS += target

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive
