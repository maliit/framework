MALIIT_INTERFACE_VERSION = 0.20

!nomeegotouch {
    include(defines-legacy.pri)
} else {
    include(defines.pri)
}

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

M_IM_PLUGINS_DIR = $$M_IM_INSTALL_LIBS/$$MALIIT_PLUGINS
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

defineTest(outputFile) {
    out = $$OUT_PWD/$$1
    in = $$PWD/$${1}.in

    !exists($$in) {
        error($$in does not exist!)
        return(false)
    }

    variables = M_IM_FRAMEWORK_FEATURE \
                M_IM_PREFIX \
                M_IM_INSTALL_BIN \
                M_IM_INSTALL_HEADERS \
                M_IM_INSTALL_LIBS \
                M_IM_PLUGINS_DIR \
                M_IM_VERSION \
                M_IM_ENABLE_MULTITOUCH \
                M_IM_DEFAULT_PLUGIN \
                M_IM_DEFAULT_SUBVIEW \
                M_IM_QUICK_FEATURE \
                MALIIT_PLUGINS_HEADER \

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
