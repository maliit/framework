include(../config.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = dummy

outputFiles(maliit-sdk)

OTHER_FILES = \
    maliit-sdk-create.sh \
    maliit-sdk.in

# Build
MODE = normal

DUMMY = maliit-sdk

SDK_BUILD_DIR = $$OUT_PWD/build/maliit-sdk/
# qmake creates wrong install rules for directories
# that do not exist at qmake time, so we hack it here
system(mkdir -p $$SDK_BUILD_DIR)

# When creating the SDK, qmake will complain (and fail on newer Qt)
# if the referenced pkg-config IDs cannot be found
PKG_CONFIG_PATH = $$OUT_PWD/../maliit/:$$OUT_PWD/../maliit-settings/:$$OUT_PWD/../maliit-glib/:$$OUT_PWD/../src/:$$OUT_PWD/../common/:$$OUT_PWD/../connection/:$$OUT_PWD/../maliit-plugins-quick/input-method

sdk.name = create_sdk
sdk.CONFIG += target_predeps no_link
sdk.commands += PKG_CONFIG_PATH=$$PKG_CONFIG_PATH $$IN_PWD/maliit-sdk-create.sh $$IN_PWD/.. $$OUT_PWD/build $$MALIIT_VERSION $$MODE
sdk.output = $$SDK_BUILD_DIR/examples
sdk.clean_commands = rm -rf $$SDK_BUILD_DIR/examples
sdk.input = DUMMY

QMAKE_EXTRA_COMPILERS += sdk

# Install
sdk_install.files = $$SDK_BUILD_DIR
sdk_install.path = $$DOCDIR/$$MALIIT_PACKAGENAME
sdk_install.CONFIG += no_check_exist directory
sdk_install.depends += build

sdk_executable.CONFIG += no_check_exist
sdk_executable.files = $$OUT_PWD/maliit-sdk
sdk_executable.path = $$BINDIR

INSTALLS += sdk_install sdk_executable

# TODO: Extract and build SDK as part of a distcheck
