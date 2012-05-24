TEMPLATE = lib
TARGET = dummy

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    python.files = maliit-exampleapp-gtk3-python.py
    python.path = $$M_IM_INSTALL_BIN
    INSTALLS += python
}
