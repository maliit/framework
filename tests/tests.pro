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
          bbt_connection \
          ut_mimonscreenplugins \
          ut_minputmethodquickplugin \

x11 {
    SUBDIRS += \
          ut_mimapplication \
          ut_passthroughserver \
          ut_selfcompositing \
          ut_mimrotationanimation \
          ut_mimserveroptions \ #FIXME: split out the common tests (non X11 dependent), so that they are always tested
}

SUBDIRS += \
          ut_mimpluginmanager \
          ut_mimpluginmanagerconfig \
          ft_mimpluginmanager \

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive
