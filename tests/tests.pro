include(../config.pri)

CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS += \
          dummyimplugin \
          dummyimplugin2 \
          dummyimplugin3 \
          dummyplugin \
          sanitychecks \
          ut_mattributeextensionmanager \
          ut_mkeyoverride \
          ft_exampleplugin \
          ut_mimsettings \
          ut_mimonscreenplugins \
          ut_minputmethodquickplugin \
          ut_mimserveroptions \

SUBDIRS += \
          ut_mimpluginmanager \
          ut_mimpluginmanagerconfig \
          ft_mimpluginmanager \

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive
