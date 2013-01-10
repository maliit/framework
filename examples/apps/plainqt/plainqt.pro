TEMPLATE = app
TARGET = maliit-exampleapp-plainqt

OBJECTS_DIR = .obj
MOC_DIR = .moc

DEPENDPATH += .

BUILD_TYPE = unittest

!contains(QT_MAJOR_VERSION, 5) {
    contains(BUILD_TYPE, skeleton) {
        CONFIG += link_pkgconfig
        PKGCONFIG += maliit
        INCLUDEPATH += $$system(pkg-config --cflags maliit | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
    }
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    target.path = $$BINDIR
    INSTALLS += target

    !contains(QT_MAJOR_VERSION, 5) {
        # Used for testing purposes, can be deleted when used as a project skeleton
        # Build against in-tree libs
        TOP_DIR = ../../..
        include($$TOP_DIR/common/libmaliit-common.pri)
        include($$TOP_DIR/maliit/libmaliit.pri)
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
