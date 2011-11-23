# To avoid qmake from cleaning and trying to build
# the .qml file with g++....
TEMPLATE = lib
TARGET = dummy

PLUGIN_FILE = helloworld.qml

plugin.files = $$PLUGIN_FILE # Install target
# plugin.path # Different depending on BUILD_TYPE

OTHER_FILES = $$PLUGIN_FILE

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-plugins-quick-0.80
    plugin.path += $$system(pkg-config --variable pluginsdir maliit-plugins-0.80)
    INSTALLS += plugin
}

contains(BUILD_TYPE, skeleton-legacy) {
    CONFIG += meegoimframework
    plugin.path += $$system(pkg-config --variable pluginsdir MeegoImFramework)
    INSTALLS += plugin
}

contains(BUILD_TYPE, unittest) {
    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    TOP_DIR = ../../../..

    include($$TOP_DIR/config.pri)

    plugin.path += $$MALIIT_TEST_PLUGINS_DIR/examples/qml/helloworld
    INSTALLS += plugin
}
