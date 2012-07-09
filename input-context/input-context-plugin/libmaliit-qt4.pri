# Should normally not be used! This is a plugin.
#
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

INCLUDEPATH += $$TOP_DIR/input-context/input-context-plugin
LIBS += -L$$TOP_DIR/lib/plugins \
        -l$$MALIIT_INPUTCONTEXTPLUGIN_TARGETNAME
POST_TARGETDEPS += $$TOP_DIR/lib/plugins/$$maliitDynamicLib($$MALIIT_INPUTCONTEXTPLUGIN_TARGETNAME)
