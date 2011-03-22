
CONFIG += ordered
TARGET = meego-im-uiserver
TEMPLATE = subdirs
SUBDIRS = src passthroughserver input-context demos tests

isEqual( IN_PWD, $${OUT_PWD} ) {
    IS_OUT_OF_SOURCE = 0
} else {
    IS_OUT_OF_SOURCE = 1
}

CONFIG(docs) {
    include (doc/doc.pri)
}

contains(CONFIG, nomeegotouch) {
} else {
    CONFIG  += meegotouch
    SUBDIRS += translations settings-applet
}

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

GCONF_DATA = meego-im-framework.schemas
gconf_data.path = /usr/share/gconf/schemas
gconf_data.files = $$GCONF_DATA

INSTALLS += gconf_data
