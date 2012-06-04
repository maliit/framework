TEMPLATE = app
TARGET = maliit-exampleapp-settings

OBJECTS_DIR = .obj
MOC_DIR = .moc

DEPENDPATH += .

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-settings-1.0
    INCLUDEPATH += $$system(pkg-config --cflags maliit-1.0 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    target.path = $$BINDIR
    INSTALLS += target

    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    TOP_DIR = ../../..
    include($$TOP_DIR/common/libmaliit-common.pri)
    include($$TOP_DIR/maliit/libmaliit.pri)
    include($$TOP_DIR/maliit-settings/libmaliit-settings.pri)
}

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    mainwindow.h \

QMAKE_CLEAN += maliit-exampleapp-settings
