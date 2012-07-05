TEMPLATE = lib
TARGET = dummy

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    python.files = maliit-exampleapp-settings-python3.py
    python.path = $$BINDIR
    INSTALLS += python
}

EXTRA_FILES = maliit-exampleapp-settings-python3.py
