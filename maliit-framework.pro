include(./config.pri)

CONFIG += ordered
TARGET = meego-im-uiserver
TEMPLATE = subdirs
SUBDIRS = src passthroughserver

nomeegotouch {
    SUBDIRS += maliit
}

SUBDIRS += input-context input-method-quick examples

isEqual( IN_PWD, $${OUT_PWD} ) {
    IS_OUT_OF_SOURCE = 0
} else {
    IS_OUT_OF_SOURCE = 1
}

include (doc/doc.pri)

!nomeegotouch {
    CONFIG  += meegotouch
    SUBDIRS += translations
}

!nomeegotouch:!noduicontrolpanel {
    SUBDIRS += settings-applet
}

!notests {
    SUBDIRS += tests
}

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

