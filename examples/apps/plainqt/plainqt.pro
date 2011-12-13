TEMPLATE = app
TARGET = maliit-exampleapp-plainqt

OBJECTS_DIR = .obj
MOC_DIR = .moc

DEPENDPATH += .

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton)|contains(BUILD_TYPE, skeleton-legacy) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-1.0
    INCLUDEPATH += $$system(pkg-config --cflags maliit-1.0 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    target.path = $$M_IM_INSTALL_BIN
    INSTALLS += target

    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    external-libmaliit {
        CONFIG += link_pkgconfig
        PKGCONFIG += maliit-1.0
    } else {
        TOP_DIR = ../../..
        INCLUDEPATH += $$TOP_DIR $$TOP_DIR/common
        LIBS += $$TOP_DIR/maliit/lib$${MALIIT_LIB}.so
    }
}

SOURCES += \
           plainqt.cpp \
           mainwindow.cpp \

HEADERS += \
           mainwindow.h \

contains(QT_MAJOR_VERSION, 4) {
    QT += core gui
} else {
    QT += core gui widgets
}

QMAKE_CLEAN += maliit-exampleapp-plainqt
