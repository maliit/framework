TEMPLATE = app
TARGET = maliit-exampleapp-embedded

OBJECTS_DIR = .obj
MOC_DIR = .moc

DEPENDPATH += .

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-1.0
    INCLUDEPATH += $$system(pkg-config --cflags maliit-1.0 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)

    # FIXME: install MImServer header, and provide a dedicated library and .pc for it
    QMAKE_LIBDIR_QT -= /usr/lib64
    QMAKE_LIBDIR_QT -= /usr/lib
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    target.path = $$M_IM_INSTALL_BIN
    INSTALLS += target

    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    TOP_DIR = ../../..
    include($$TOP_DIR/common/libmaliit-common.pri)
    include($$TOP_DIR/maliit/libmaliit.pri)
    include($$TOP_DIR/connection/libmaliit-connection.pri)
}

SOURCES += \
    main.cpp \
    embedded.cpp \

HEADERS += \
    embedded.h \

contains(QT_MAJOR_VERSION, 4) {
    QT += core gui
} else {
    QT += core gui widgets
}

QMAKE_CLEAN += maliit-exampleapp-embedded
