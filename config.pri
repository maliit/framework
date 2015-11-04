MALIIT_VERSION = $$system(cat $$PWD/VERSION)

MALIIT_ABI_VERSION = $$MALIIT_VERSION

# For libmaliit
MALIIT_LIB = maliit
MALIIT_SETTINGS_LIB = maliit-settings
MALIIT_HEADER = maliit
MALIIT_FRAMEWORK_HEADER = maliit/framework
# For libmaliit-glib
MALIIT_CONNECTION_GLIB_LIB = maliit-connection-glib
MALIIT_GLIB_LIB = maliit-glib

include(defines.pri)

# Linker optimization for release build
QMAKE_LFLAGS_RELEASE+=-Wl,--as-needed
# Compiler warnings are error if the build type is debug
QMAKE_CXXFLAGS_DEBUG+=-Werror -O0
QMAKE_CFLAGS_DEBUG+=-Werror -O0

OBJECTS_DIR = .obj
MOC_DIR = .moc

isEmpty(MALIIT_VERSION) {
    MALIIT_VERSION=$$MALIIT_VERSION
}

isEmpty(PREFIX) {
    PREFIX = /usr
}
isEmpty(BINDIR) {
    BINDIR = $$PREFIX/bin
}
isEmpty(LIBDIR) {
    LIBDIR = $$PREFIX/lib
}
isEmpty(INCLUDEDIR) {
    INCLUDEDIR = $$PREFIX/include
}
DATADIR = $$PREFIX/share

isEmpty(DOCDIR) {
    DOCDIR = $$DATADIR/doc
}

isEmpty(MALIIT_PLUGINS_DIR) {
    MALIIT_PLUGINS_DIR = $$LIBDIR/$$MALIIT_PLUGINS
}
DEFINES += MALIIT_PLUGINS_DIR=\\\"$$MALIIT_PLUGINS_DIR\\\"

isEmpty(MALIIT_PLUGINS_DATA_DIR) {
    MALIIT_PLUGINS_DATA_DIR = $$DATADIR/$$MALIIT_PLUGINS_DATA
}
DEFINES += MALIIT_PLUGINS_DATA_DIR=\\\"$$MALIIT_PLUGINS_DATA_DIR\\\"

isEmpty(MALIIT_FACTORY_PLUGINS_DIR) {
    MALIIT_FACTORY_PLUGINS_DIR = $$MALIIT_PLUGINS_DIR/factories
}
DEFINES += MALIIT_FACTORY_PLUGINS_DIR=\\\"$$MALIIT_FACTORY_PLUGINS_DIR\\\"

isEmpty(MALIIT_ENABLE_MULTITOUCH) {

    MALIIT_ENABLE_MULTITOUCH=true
}

MALIIT_EXTENSIONS_DIR = $$DATADIR/$$MALIIT_ATTRIBUTE_EXTENSIONS/
DEFINES += MALIIT_EXTENSIONS_DIR=\\\"$$MALIIT_EXTENSIONS_DIR\\\"

isEmpty(MALIIT_DEFAULT_HW_PLUGIN) {
    MALIIT_DEFAULT_HW_PLUGIN = libmaliit-keyboard-plugin.so
}

isEmpty(MALIIT_DEFAULT_PLUGIN) {
    MALIIT_DEFAULT_PLUGIN = libmaliit-keyboard-plugin.so
}

MALIIT_TEST_DATADIR = $$DATADIR/$$MALIIT_TEST_SUITE
MALIIT_TEST_LIBDIR = $$LIBDIR/$$MALIIT_TEST_SUITE
MALIIT_TEST_TMPDIR = /tmp/$$MALIIT_TEST_SUITE
MALIIT_TEST_PLUGINS_DIR = $$MALIIT_TEST_LIBDIR/plugins

DEFINES += MALIIT_TEST_PLUGINS_DIR=\\\"$$MALIIT_TEST_PLUGINS_DIR\\\"

DEFINES += MALIIT_CONFIG_ROOT=\\\"$$MALIIT_CONFIG_ROOT\\\"

DEFINES += MALIIT_FRAMEWORK_USE_INTERNAL_API

# Do not define keywords signals, slots, emit and foreach because of
# conflicts with 3rd party libraries.
CONFIG += no_keywords

unix {
    MALIIT_STATIC_PREFIX=lib
    MALIIT_STATIC_SUFFIX=.a
    MALIIT_DYNAMIC_PREFIX=lib
    MALIIT_DYNAMIC_SUFFIX=.so
    MALIIT_ABI_VERSION_MAJOR=
}

win32 {
    # qmake puts libraries in subfolders in build tree on Windows (installation is unaffected)
    release {
        MALIIT_STATIC_PREFIX=release/lib
        MALIIT_DYNAMIC_PREFIX=release/
    }
    debug {
        MALIIT_STATIC_PREFIX=debug/lib
        MALIIT_DYNAMIC_PREFIX=debug/
    }

    # one would suspect this to be .lib, but qmake with mingw uses .a
    MALIIT_STATIC_SUFFIX=.a

    # qmake adds the first component of the version as part of the DLL name on Windows
    MALIIT_ABI_VERSIONS=$$split(MALIIT_ABI_VERSION, ".")
    MALIIT_ABI_VERSION_MAJOR=$$member(MALIIT_ABI_VERSIONS, 0)
    MALIIT_DYNAMIC_SUFFIX=$${MALIIT_ABI_VERSION_MAJOR}.dll
}

defineReplace(maliitStaticLib) {
    return($${MALIIT_STATIC_PREFIX}$${1}$${MALIIT_STATIC_SUFFIX})
}

defineReplace(maliitDynamicLib) {
    return($${MALIIT_DYNAMIC_PREFIX}$${1}$${MALIIT_DYNAMIC_SUFFIX})
}

mac {
    # Do mac stuff here
    M_BUILD_FEATURES = debug
    M_PROFILE_PARTS =
    INCLUDEPATH += include/
}

contains(QT_CONFIG,embedded) {
    CONFIG += qws
}

wayland {
    DEFINES += HAVE_WAYLAND
}

noxcb {
    DEFINES += NOXCB
}

MALIIT_INSTALL_PRF = $$[QT_INSTALL_DATA]/mkspecs/features
local-install {
    MALIIT_INSTALL_PRF = $$replace(MALIIT_INSTALL_PRF, $$[QT_INSTALL_PREFIX], $$PREFIX)
}

defineTest(outputFile) {
    out = $$OUT_PWD/$$1
    in = $$PWD/$${1}.in

    !exists($$in) {
        error($$in does not exist!)
        return(false)
    }

    MALIIT_IN_DIR = $$PWD
    MALIIT_OUT_DIR = $$OUT_PWD

    variables = MALIIT_FRAMEWORK_FEATURE \
                PREFIX \
                BINDIR \
                INCLUDEDIR \
                LIBDIR \
                DOCDIR \
                MALIIT_PLUGINS_DIR \
                MALIIT_PLUGINS_DATA_DIR \
                MALIIT_FACTORY_PLUGINS_DIR \
                MALIIT_VERSION \
                MALIIT_ENABLE_MULTITOUCH \
                MALIIT_DEFAULT_HW_PLUGIN \
                MALIIT_DEFAULT_PLUGIN \
                MALIIT_QUICK_FEATURE \
                MALIIT_PLUGINS_LIB \
                MALIIT_PLUGINS_HEADER \
                MALIIT_LIB \
                MALIIT_HEADER \
                MALIIT_PLUGINS_QUICK_LIB \
                MALIIT_PLUGINS_QUICK_HEADER \
                MALIIT_PLUGINS_QUICK_FACTORY \
                MALIIT_SETTINGS_LIB \
                MALIIT_TEST_SUITE \
                MALIIT_TEST_DATADIR \
                MALIIT_TEST_LIBDIR \
                MALIIT_TEST_TMPDIR \
                MALIIT_IN_DIR \
                MALIIT_OUT_DIR \
                MALIIT_PACKAGENAME \
                MALIIT_PACKAGE_BRIEF \
                MALIIT_FRAMEWORK_HEADER \
                MALIIT_SERVER_ARGUMENTS \
                MALIIT_CONNECTION_LIB \
                MALIIT_SERVER_HEADER \
                MALIIT_ABI_VERSION_MAJOR \
                MALIIT_GLIB_HEADER \
                MALIIT_GLIB_LIB \

    command = "sed"
    for(var, variables) {
       command += "-e \"s;@$$var@;$$eval($$var);g\""
    }
    command += $$in > $$out

    system(mkdir -p $$dirname(out))
    system($$command)
    system(chmod --reference=$$in $$out)

    QMAKE_DISTCLEAN += $$1

    export(QMAKE_DISTCLEAN)

    return(true)
}

defineTest(outputFiles) {
    files = $$ARGS

    for(file, files) {
        !outputFile($$file):return(false)
    }

    return(true)
}
