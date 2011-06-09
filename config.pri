# Linker optimization for release build
QMAKE_LFLAGS_RELEASE+=-Wl,--as-needed
# Compiler warnings are error if the build type is debug
QMAKE_CXXFLAGS_DEBUG+=-Werror

isEmpty(M_IM_VERSION) {
    M_IM_VERSION=$$system(sed -e \'/^meego-im-framework/!d\'  -e \'s/^meego-im-framework (\\([^-~]*\\)[-~].*).*$/\\1/\' $$PWD/debian/changelog | head -1)
}

isEmpty(M_IM_PREFIX) {
    M_IM_PREFIX = /usr
}
isEmpty(M_IM_INSTALL_BIN) {
    M_IM_INSTALL_BIN = $$M_IM_PREFIX/bin
}
isEmpty(M_IM_INSTALL_LIBS) {
    M_IM_INSTALL_LIBS = $$M_IM_PREFIX/lib
}
isEmpty(M_IM_INSTALL_HEADERS) {
    M_IM_INSTALL_HEADERS = $$M_IM_PREFIX/include
}
isEmpty(M_IM_INSTALL_SCHEMAS) {
    M_IM_INSTALL_SCHEMAS = /usr/share/gconf/schemas
}

M_IM_PLUGINS_DIR = $$M_IM_INSTALL_LIBS/meego-im-plugins

DEFINES += M_IM_PLUGINS_DIR=\\\"$$M_IM_PLUGINS_DIR\\\"

isEmpty(M_IM_ENABLE_MULTITOUCH) {
    M_IM_ENABLE_MULTITOUCH=true
}

!nomeegotouch {
    M_IM_DEFAULT_PLUGIN = libmeego-keyboard.so
    M_IM_DEFAULT_SUBVIEW = en_gb.xml
} else {
    M_IM_DEFAULT_PLUGIN = libmeego-keyboard-quick.so
    M_IM_DEFAULT_SUBVIEW = 
}

mac {
    # Do mac stuff here
    M_BUILD_FEATURES = debug
    M_PROFILE_PARTS =
    INCLUDEPATH += include/
}
