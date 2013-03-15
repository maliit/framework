include(../config.pri)

TOP_DIR = ..

VERSION = $$MALIIT_ABI_VERSION
TEMPLATE = lib
TARGET = $$TOP_DIR/lib/$$MALIIT_WESTON_PROTOCOLS_LIB

CONFIG += staticlib

wayland {
    load(wayland-scanner)

    # to force generation of headers.
    SOURCES = dummy.cpp

    WAYLANDSOURCES += \
        $$IN_PWD/input-method.xml \
        $$IN_PWD/text.xml
    CONFIG += link_pkgconfig
    PKGCONFIG += wayland-client
}

OTHER_FILES += libmaliit-weston-protocols.pri
