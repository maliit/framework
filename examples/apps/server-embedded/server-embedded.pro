TEMPLATE = app
TARGET = maliit-example-embedded-server

OBJECTS_DIR = .obj
MOC_DIR = .moc

DEPENDPATH += .

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton)|contains(BUILD_TYPE, skeleton-legacy) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-server-0.80
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    target.path = $$M_IM_INSTALL_BIN
    INSTALLS += target

    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    TOP_DIR = ../../..
    INCLUDEPATH += $$TOP_DIR $$TOP_DIR/common $$TOP_DIR/src
    LIBS += $$TOP_DIR/src/lib$${MALIIT_PLUGINS_LIB}.so

    include($$TOP_DIR/connection/libmaliit-connection.pri)
}

SOURCES += \
    main.cpp \

HEADERS += \

contains(QT_MAJOR_VERSION, 4) {
    QT += core gui
} else {
    QT += core gui widgets
}

QMAKE_CLEAN += maliit-example-embedded-server
