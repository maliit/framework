MALIIT_VERSION = 0.80.7
MALIIT_INTERFACE_VERSION = 1.0
MALIIT_PLUGINS_INTERFACE_VERSION = 0.80
MALIIT_PLUGINS_QUICK_INTERFACE_VERSION = 0.80
MALIIT_FRAMEWORK_INTERFACE_VERSION = 0.80

# For libmaliit
MALIIT_LIB = maliit-$${MALIIT_INTERFACE_VERSION}
MALIIT_HEADER = maliit/maliit-$${MALIIT_INTERFACE_VERSION}
MALIIT_FRAMEWORK_HEADER = maliit/framework-$${MALIIT_FRAMEWORK_INTERFACE_VERSION}

enable-legacy {
    include(defines-legacy.pri)
} else {
    include(defines.pri)
}

# Linker optimization for release build
QMAKE_LFLAGS_RELEASE+=-Wl,--as-needed
# Compiler warnings are error if the build type is debug
QMAKE_CXXFLAGS_DEBUG+=-Werror -O0
QMAKE_CFLAGS_DEBUG+=-Werror -O0

OBJECTS_DIR = .obj
MOC_DIR = .moc

isEmpty(M_IM_VERSION) {
    M_IM_VERSION=$$MALIIT_VERSION
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
DATADIR = $$M_IM_PREFIX/share
isEmpty(M_IM_INSTALL_SCHEMAS) {
    M_IM_INSTALL_SCHEMAS = $$DATADIR/gconf/schemas
}

isEmpty(M_IM_INSTALL_DOCS) {
    M_IM_INSTALL_DOCS = $$DATADIR/doc
}

M_IM_PLUGINS_DIR = $$M_IM_INSTALL_LIBS/$$MALIIT_PLUGINS
DEFINES += M_IM_PLUGINS_DIR=\\\"$$M_IM_PLUGINS_DIR\\\"

M_IM_FACTORY_PLUGINS_DIR = $$M_IM_PLUGINS_DIR/factories
DEFINES += M_IM_FACTORY_PLUGINS_DIR=\\\"$$M_IM_FACTORY_PLUGINS_DIR\\\"

isEmpty(M_IM_ENABLE_MULTITOUCH) {

    M_IM_ENABLE_MULTITOUCH=true
}

MALIIT_EXTENSIONS_DIR = $$DATADIR/$$MALIIT_ATTRIBUTE_EXTENSIONS/
DEFINES += MALIIT_EXTENSIONS_DIR=\\\"$$MALIIT_EXTENSIONS_DIR\\\"

enable-legacy {
    M_IM_DEFAULT_PLUGIN = libmeego-keyboard.so
    M_IM_DEFAULT_SUBVIEW = en_gb.xml
} else {
    M_IM_DEFAULT_PLUGIN = libmeego-keyboard-quick.so
    M_IM_DEFAULT_SUBVIEW = 
}

MALIIT_TEST_DATADIR = $$DATADIR/$$MALIIT_TEST_SUITE
MALIIT_TEST_LIBDIR = $$M_IM_INSTALL_LIBS/$$MALIIT_TEST_SUITE
MALIIT_TEST_TMPDIR = /tmp/$$MALIIT_TEST_SUITE
MALIIT_TEST_PLUGINS_DIR = $$MALIIT_TEST_LIBDIR/plugins

DEFINES += MALIIT_TEST_PLUGINS_DIR=\\\"$$MALIIT_TEST_PLUGINS_DIR\\\"

DEFINES += MALIIT_CONFIG_ROOT=\\\"$$MALIIT_CONFIG_ROOT\\\"

DEFINES += MALIIT_FRAMEWORK_USE_INTERNAL_API

# Do not define keywords signals, slots, emit and foreach because of
# conflicts with 3rd party libraries.
CONFIG += no_keywords

mac {
    # Do mac stuff here
    M_BUILD_FEATURES = debug
    M_PROFILE_PARTS =
    INCLUDEPATH += include/
}

# Use x11 on unix systems without lighthouse (qpa)
unix:!qpa: {
    CONFIG += x11
}

enable-legacy {
    DEFINES += HAVE_LEGACY_NAMES
}

defineTest(outputFile) {
    out = $$OUT_PWD/$$1
    in = $$PWD/$${1}.in

    !exists($$in) {
        error($$in does not exist!)
        return(false)
    }

    MALIIT_IN_DIR = $$IN_PWD
    MALIIT_OUT_DIR = $$OUT_PWD

    variables = M_IM_FRAMEWORK_FEATURE \
                M_IM_PREFIX \
                M_IM_INSTALL_BIN \
                M_IM_INSTALL_HEADERS \
                M_IM_INSTALL_LIBS \
                M_IM_PLUGINS_DIR \
                M_IM_FACTORY_PLUGINS_DIR \
                M_IM_VERSION \
                M_IM_ENABLE_MULTITOUCH \
                M_IM_DEFAULT_PLUGIN \
                M_IM_DEFAULT_SUBVIEW \
                M_IM_QUICK_FEATURE \
                MALIIT_PLUGINS_LIB \
                MALIIT_PLUGINS_HEADER \
                MALIIT_LIB \
                MALIIT_HEADER \
                MALIIT_PLUGINS_QUICK_LIB \
                MALIIT_PLUGINS_QUICK_HEADER \
                MALIIT_TEST_SUITE \
                MALIIT_TEST_DATADIR \
                MALIIT_TEST_LIBDIR \
                MALIIT_TEST_TMPDIR \
                MALIIT_IN_DIR \
                MALIIT_OUT_DIR \
                MALIIT_PACKAGE_BRIEF \
                MALIIT_FRAMEWORK_HEADER \

    command = "sed"
    for(var, variables) {
       command += "-e \"s:@$$var@:$$eval($$var):g\""
    }
    command += $$in > $$out

    system(mkdir -p $$dirname(out))
    system($$command)
    system(chmod --reference=$$in $$out)

    return(true)
}

defineTest(outputFiles) {
    files = $$ARGS

    for(file, files) {
        !outputFile($$file):return(false)
    }

    return(true)
}
