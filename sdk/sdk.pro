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

SDK_BUILD_DIR = $$OUT_PWD/build/maliit-sdk/
# qmake creates wrong install rules for directories
# that do not exist at qmake time, so we hack it here
system(mkdir -p $$SDK_BUILD_DIR)

sdk.name = create_sdk
sdk.CONFIG += target_predeps no_link
sdk.commands += $$IN_PWD/maliit-sdk-create.sh $$IN_PWD/.. $$OUT_PWD/build $$MALIIT_VERSION $$MODE
sdk.output = $$SDK_BUILD_DIR/examples
sdk.clean_commands = rm -rf $$SDK_BUILD_DIR/examples
sdk.input = DUMMY

QMAKE_EXTRA_COMPILERS += sdk

# Install
sdk_install.files = $$SDK_BUILD_DIR
sdk_install.path = $$M_IM_INSTALL_DOCS/$$MALIIT_PACKAGENAME
sdk_install.CONFIG += no_check_exist directory
sdk_install.depends += build

sdk_executable.CONFIG += no_check_exist
sdk_executable.files = $$OUT_PWD/maliit-sdk
sdk_executable.path = $$M_IM_INSTALL_BIN

INSTALLS += sdk_install sdk_executable

# TODO: Extract and build SDK as part of a distcheck
