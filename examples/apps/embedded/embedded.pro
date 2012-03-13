TEMPLATE = app
TARGET = maliit-exampleapp-embedded

OBJECTS_DIR = .obj
MOC_DIR = .moc

DEPENDPATH += .

BUILD_TYPE = unittest

contains(BUILD_TYPE, skeleton) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-1.0 maliit-plugins-0.80
    INCLUDEPATH += $$system(pkg-config --cflags maliit-1.0 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)
    INCLUDEPATH += $$system(pkg-config --cflags maliit-plugins-0.80 | tr \' \' \'\\n\' | grep ^-I | cut -d I -f 2-)

    # FIXME: install MImServer header, and provide a dedicated library and .pc for it
}

contains(BUILD_TYPE, unittest) {
    include(../../../config.pri)
    target.path = $$M_IM_INSTALL_BIN
    INSTALLS += target

    # Used for testing purposes, can be deleted when used as a project skeleton
    # Build against in-tree libs
    TOP_DIR = ../../..
    INCLUDEPATH += $$TOP_DIR $$TOP_DIR/common $$TOP_DIR/src
    LIBS += $$TOP_DIR/maliit/lib$${MALIIT_LIB}.so

    INCLUDEPATH += $$TOP_DIR $$TOP_DIR/common $$TOP_DIR/connection
    LIBS += $$TOP_DIR/src/lib$${MALIIT_PLUGINS_LIB}.so

    LIBS += $$TOP_DIR/connection/libmaliit-connection.a
    POST_TARGETDEPS += $$TOP_DIR/connection/libmaliit-connection.a

    #FIXME: a better way to get this
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
    PKGCONFIG += dbus-glib-1 gio-2.0
    CONFIG += qdbus
    # needs to match what the connection lib uses

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
