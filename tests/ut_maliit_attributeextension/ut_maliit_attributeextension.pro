include(../common_top.pri)

SOURCES += ut_maliit_attributeextension.cpp

external-libmaliit {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-1.0
} else {
    LIBS += \
        $$TOP_DIR/maliit/libmaliit-1.0.so
}

include(../common_check.pri)
