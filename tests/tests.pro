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
          ut_mtoolbarmanager \
          ft_mimsettingsapplet \
          ut_mimsettingsconf \
          ut_mimsettingsdialog \

contains(DEFINES, QT_DBUS) {
SUBDIRS += ut_minputcontextdbusconnection
}

target.commands += $$system(touch tests.xml)
target.path = /usr/share/meego-im-framework-tests
target.files += qtestlib2junitxml.xsl runtests.sh tests.xml
INSTALLS += target

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

