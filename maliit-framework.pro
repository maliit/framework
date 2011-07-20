include(./config.pri)

CONFIG += ordered
TARGET = meego-im-uiserver
TEMPLATE = subdirs
SUBDIRS = src passthroughserver maliit

SUBDIRS += input-context input-method-quick examples

!nodoc {
    SUBDIRS += doc
}

!nomeegotouch {
    CONFIG  += meegotouch
}

!notests {
    SUBDIRS += tests
}

!system(pkg-config --exists dbus-glib-1 dbus-1 gconf-2.0):error("Could not find dbus-glib-1 dbus-1 gconf-2.0")

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

