
LIBS += \
    -L$$TOP_DIR/tests/plugins \
    -ldummyimplugin \
    -ldummyimplugin2 \
    -ldummyimplugin3 \

INCLUDEPATH += \
    $$TOP_DIR/tests/dummyimplugin \
    $$TOP_DIR/tests/dummyimplugin2 \
    $$TOP_DIR/tests/dummyimplugin3 \
