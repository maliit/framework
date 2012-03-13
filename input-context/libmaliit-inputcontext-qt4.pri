# Use when a .pro file requires libmaliit-inputcontext-qt4
# The .pro file must define TOP_DIR to be a relative path
# to the top-level source/build directory, and include config.pri

INCLUDEPATH += $$TOP_DIR/input-context/maliit-input-context
LIBS += $$TOP_DIR/input-context/maliit-input-context/$$maliitStaticLib($${MALIIT_INPUTCONTEXT_TARGETNAME})
POST_TARGETDEPS += $$TOP_DIR/input-context/maliit-input-context/$$maliitStaticLib($${MALIIT_INPUTCONTEXT_TARGETNAME})
