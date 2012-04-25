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
          ut_maliit_attributeextension \
          ut_maliit_attributeextensionregistry \
          ut_maliit_inputmethod \
          ut_mimsettings \
          bbt_connection \

contains(QT_MAJOR_VERSION, 4) {
    SUBDIRS += \
          ut_minputcontext \
          ut_minputmethodquickplugin \

    !disable-dbus {
        SUBDIRS += \
              ut_minputcontextplugin \
    }
}

x11 {
    SUBDIRS += \
          ut_mimapplication \
          ut_passthroughserver \
          ut_selfcompositing \
          ut_mimrotationanimation \
          ut_mimserveroptions \ #FIXME: split out the common tests (non X11 dependent), so that they are always tested
}

!disable-dbus {
    SUBDIRS += \
          ut_mimpluginmanager \
          ft_mimpluginmanager \
}

target.CONFIG += no_check_exist
INSTALLS += target

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive
