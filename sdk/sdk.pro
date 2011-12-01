include(../config.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = dummy

outputFiles(maliit-sdk)

OTHER_FILES = \
    maliit-sdk-create.sh \
    maliit-sdk.in

# Build
enable-legacy {
    MODE = legacy
} else {
    MODE = normal
}

DUMMY = maliit-sdk

sdk.name = create_sdk
sdk.CONFIG += target_predeps no_link
sdk.commands += $$IN_PWD/maliit-sdk-create.sh $$IN_PWD/.. $$OUT_PWD/build $$MALIIT_VERSION $$MODE
sdk.output = build
sdk.clean_commands = rm -rf build
sdk.input = DUMMY

QMAKE_EXTRA_COMPILERS += sdk

# Install
sdk_install.files = $$OUT_PWD/build/maliit-sdk/
sdk_install.path = $$M_IM_INSTALL_DOCS/$$MALIIT_PACKAGENAME
sdk_install.CONFIG += no_check_exist directory
sdk_install.depends += build

sdk_executable.CONFIG += no_check_exist
sdk_executable.files = $$OUT_PWD/maliit-sdk
sdk_executable.path = $$M_IM_INSTALL_BIN

INSTALLS += sdk_install sdk_executable

# TODO: Extract and build SDK as part of a distcheck
