include(./config.pri)

CONFIG += ordered
TEMPLATE = subdirs

!only-libmaliit {
    SUBDIRS = src passthroughserver
    TARGET = meego-im-uiserver
}

external-libmaliit {
    only-libmaliit:error("CONFIG options only-libmaliit and external-libmaliit does not make sense together!")
    !system(pkg-config --exists maliit-1.0):error("Could not find maliit-1.0")
} else {
    SUBDIRS += maliit
}

!only-libmaliit {
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
}

!system(pkg-config --exists dbus-glib-1 dbus-1 gconf-2.0):error("Could not find dbus-glib-1 dbus-1 gconf-2.0")

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

OTHER_FILES += NEWS README
