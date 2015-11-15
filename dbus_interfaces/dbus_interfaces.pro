TOP_DIR = ..

include($$TOP_DIR/config.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = dummy

OTHER_FILES += \
    minputmethodcontext1interface.xml \
    minputmethodserver1interface.xml
