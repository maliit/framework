include(../config.pri)

CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS += \
          dummyimplugin \
          dummyimplugin2 \
          dummyimplugin3 \
          dummyplugin \
          sanitychecks \
          ut_mimpluginmanager \
          ft_mimpluginmanager \
          ut_mtoolbardata \
          ut_mtoolbaritem \
          ut_mattributeextensionmanager \
          ut_mkeyoverride \
          ft_exampleplugin \
          ut_maliit_attributeextension \
          ut_maliit_attributeextensionregistry \
          ut_maliit_inputmethod \
          ut_mimsettings \
          bbt_connection \

contains(QT_MAJOR_VERSION, 4) {
    SUBDIRS += \
          ut_minputcontext \
          ut_minputcontextplugin \
          ut_minputmethodquickplugin \
}

x11 {
    SUBDIRS += \
          ut_mimapplication \
          ut_passthroughserver \
          ut_selfcompositing \
          ut_mimrotationanimation \
          ut_mimserveroptions \ #FIXME: split out the common tests (non X11 dependent), so that they are always tested
}

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
