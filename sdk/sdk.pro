include(../config.pri)

TEMPLATE = subdirs

outputFiles(maliit-sdk)

OTHER_FILES = \
    maliit-sdk-create.sh \
    maliit-sdk.in

# Hack to make the sdk_tarball target run automatically
QMAKE_EXTRA_TARGETS += first
first.depends += sdk

# Build
enable-legacy {
    MODE = legacy
} else {
    MODE = normal
}
QMAKE_EXTRA_TARGETS += sdk
sdk.commands += $$IN_PWD/maliit-sdk-create.sh $$IN_PWD/.. $$OUT_PWD/build $$MALIIT_VERSION $$MODE

# Install
sdk_install.files = $$OUT_PWD/build/maliit-sdk/
sdk_install.path = $$M_IM_INSTALL_DOCS/$$MALIIT_PACKAGENAME
sdk_install.CONFIG += no_check_exist directory
sdk_install.depends = sdk

sdk_executable.CONFIG += no_check_exist
sdk_executable.files = $$OUT_PWD/maliit-sdk
sdk_executable.path = $$M_IM_INSTALL_BIN

INSTALLS += sdk_install sdk_executable

# TODO: Extract and build SDK as part of a distcheck
